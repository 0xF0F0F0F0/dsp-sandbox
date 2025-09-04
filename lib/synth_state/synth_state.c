#include "synth_state.h"
#include <string.h>
#include <stdlib.h>

// Global synthesizer state
synth_state_t g_synth_state;

// Note frequencies in Hz (C4 to B4, can be scaled for other octaves)
static const float note_frequencies[] = {
	261.63f,  // C4
	277.18f,  // C#4
	293.66f,  // D4
	311.13f,  // D#4
	329.63f,  // E4
	349.23f,  // F4
	369.99f,  // F#4
	392.00f,  // G4
	415.30f,  // G#4
	440.00f,  // A4
	466.16f,  // A#4
	493.88f	  // B4
};

void synth_state_init(void)
{
	// Clear all state
	memset(&g_synth_state, 0, sizeof(synth_state_t));

	// Set default values
	g_synth_state.current_scale	= 0;  // Major scale
	g_synth_state.current_step	= 0;
	g_synth_state.sequencer_running = false;
	g_synth_state.bpm		= 120.0f;
	g_synth_state.step_counter	= 0;

	// Initialize pattern system
	g_synth_state.current_pattern = 0;  // Start with pattern 1
	g_synth_state.active_pattern  = 0;
	g_synth_state.chain_position  = 0;
	g_synth_state.chain_length    = 0;

	// Initialize all patterns (clear all steps)
	for (int p = 0; p < PATTERN_COUNT; p++) {
		for (int i = 0; i < SEQUENCER_STEPS; i++) {
			g_synth_state.patterns[p][i].active	= false;
			g_synth_state.patterns[p][i].note_index = 0;
			g_synth_state.patterns[p][i].octave	= 0;
			g_synth_state.patterns[p][i].accent	= false;
			g_synth_state.patterns[p][i].slide	= false;
		}
		g_synth_state.pattern_chain[p] = false;
		g_synth_state.chain_order[p]   = -1;
	}

	// Set default parameter values (50% for most)
	g_synth_state.parameters[PARAM_CUTOFF]	   = 50;
	g_synth_state.parameters[PARAM_RESONANCE]  = 50;
	g_synth_state.parameters[PARAM_ENVELOPE]   = 50;
	g_synth_state.parameters[PARAM_ACCENT]	   = 50;
	g_synth_state.parameters[PARAM_ATTACK]	   = 50;
	g_synth_state.parameters[PARAM_DECAY]	   = 50;
	g_synth_state.parameters[PARAM_SUSTAIN]	   = 50;
	g_synth_state.parameters[PARAM_RELEASE]	   = 50;
	g_synth_state.parameters[PARAM_DISTORTION] = 50;
	g_synth_state.parameters[PARAM_DELAY]	   = 50;
	g_synth_state.parameters[PARAM_REVERB]	   = 50;
	g_synth_state.parameters[PARAM_VOLUME]	   = 80;  // Default volume higher
}

void synth_state_reset_sequence(void)
{
	// Clear current pattern steps
	int pattern = g_synth_state.current_pattern;
	for (int i = 0; i < SEQUENCER_STEPS; i++) {
		g_synth_state.patterns[pattern][i].active     = false;
		g_synth_state.patterns[pattern][i].note_index = 0;
		g_synth_state.patterns[pattern][i].octave     = 0;  // Default octave (0 = C4 base)
		g_synth_state.patterns[pattern][i].accent     = false;
		g_synth_state.patterns[pattern][i].slide      = false;
	}
	g_synth_state.current_step = 0;
}

void synth_state_set_parameter(synth_param_t param, uint8_t value)
{
	if (param < PARAM_COUNT && value <= 100) {
		g_synth_state.parameters[param] = value;
	}
}

uint8_t synth_state_get_parameter(synth_param_t param)
{
	if (param < PARAM_COUNT) {
		return g_synth_state.parameters[param];
	}
	return 0;
}

void synth_state_set_step(int step_index, bool active, int note_index, bool accent, bool slide)
{
	if (step_index >= 0 && step_index < SEQUENCER_STEPS) {
		int pattern					       = g_synth_state.current_pattern;
		g_synth_state.patterns[pattern][step_index].active     = active;
		g_synth_state.patterns[pattern][step_index].note_index = note_index;
		g_synth_state.patterns[pattern][step_index].accent     = accent;
		g_synth_state.patterns[pattern][step_index].slide      = slide;
	}
}

sequencer_step_t* synth_state_get_step(int step_index)
{
	if (step_index >= 0 && step_index < SEQUENCER_STEPS) {
		int pattern = g_synth_state.current_pattern;
		return &g_synth_state.patterns[pattern][step_index];
	}
	return NULL;
}

void synth_state_set_scale(int scale_index)
{
	if (scale_index >= 0 && scale_index < get_scale_count()) {
		g_synth_state.current_scale = scale_index;
	}
}

int synth_state_get_scale(void)
{
	return g_synth_state.current_scale;
}

float synth_state_get_parameter_normalized(synth_param_t param)
{
	if (param < PARAM_COUNT) {
		return (float)g_synth_state.parameters[param] / 100.0f;
	}
	return 0.0f;
}

int synth_state_get_note_frequency(int step_index)
{
	if (step_index < 0 || step_index >= SEQUENCER_STEPS) {
		return 0;
	}

	// Use active pattern for DSP (the one currently playing)
	int		  pattern = g_synth_state.active_pattern;
	sequencer_step_t* step	  = &g_synth_state.patterns[pattern][step_index];
	if (!step->active) {
		return 0;  // Step is off
	}

	// Get the note from the current scale
	const int* intervals = get_scale_intervals(g_synth_state.current_scale);
	if (step->note_index >= 0 && step->note_index < 8 && intervals[step->note_index] >= 0) {
		int   note_index = intervals[step->note_index] % 12;
		float base_freq	 = note_frequencies[note_index];

		// Apply octave offset: each octave doubles/halves the frequency
		for (int i = 0; i < abs(step->octave); i++) {
			if (step->octave > 0) {
				base_freq *= 2.0f;  // Octave up
			} else if (step->octave < 0) {
				base_freq /= 2.0f;  // Octave down
			}
		}

		return (int)base_freq;
	}

	return (int)note_frequencies[0];  // Default to C4
}

void synth_state_advance_step(void)
{
	g_synth_state.step_counter++;
	g_synth_state.current_step = (g_synth_state.current_step + 1) % SEQUENCER_STEPS;

	// Handle pattern chaining
	if (g_synth_state.current_step == 0 && g_synth_state.chain_length > 1) {
		// End of pattern - advance to next pattern in chain
		g_synth_state.chain_position = (g_synth_state.chain_position + 1) % g_synth_state.chain_length;
		g_synth_state.active_pattern = g_synth_state.chain_order[g_synth_state.chain_position];
	}
}

// Pattern management functions
void synth_state_set_current_pattern(int pattern_index)
{
	if (pattern_index >= 0 && pattern_index < PATTERN_COUNT) {
		g_synth_state.current_pattern = pattern_index;
	}
}

int synth_state_get_current_pattern(void)
{
	return g_synth_state.current_pattern;
}

sequencer_step_t* synth_state_get_step_from_pattern(int pattern_index, int step_index)
{
	if (pattern_index >= 0 && pattern_index < PATTERN_COUNT && step_index >= 0 && step_index < SEQUENCER_STEPS) {
		return &g_synth_state.patterns[pattern_index][step_index];
	}
	return NULL;
}

sequencer_step_t* synth_state_get_current_step(int step_index)
{
	return synth_state_get_step_from_pattern(g_synth_state.current_pattern, step_index);
}

void synth_state_chain_pattern(int pattern_index)
{
	if (pattern_index < 0 || pattern_index >= PATTERN_COUNT)
		return;

	if (!g_synth_state.pattern_chain[pattern_index]) {
		// Add pattern to chain
		g_synth_state.pattern_chain[pattern_index]	      = true;
		g_synth_state.chain_order[g_synth_state.chain_length] = pattern_index;
		g_synth_state.chain_length++;

		// Set active pattern to first in chain
		if (g_synth_state.chain_length == 1) {
			g_synth_state.active_pattern = pattern_index;
			g_synth_state.chain_position = 0;
		}
	}
}

void synth_state_unchain_pattern(int pattern_index)
{
	if (pattern_index < 0 || pattern_index >= PATTERN_COUNT)
		return;

	if (g_synth_state.pattern_chain[pattern_index]) {
		// Remove pattern from chain
		g_synth_state.pattern_chain[pattern_index] = false;

		// Find and remove from chain order
		int remove_pos = -1;
		for (int i = 0; i < g_synth_state.chain_length; i++) {
			if (g_synth_state.chain_order[i] == pattern_index) {
				remove_pos = i;
				break;
			}
		}

		if (remove_pos >= 0) {
			// Shift remaining patterns down
			for (int i = remove_pos; i < g_synth_state.chain_length - 1; i++) {
				g_synth_state.chain_order[i] = g_synth_state.chain_order[i + 1];
			}
			g_synth_state.chain_length--;

			// Adjust chain position if needed
			if (g_synth_state.chain_position >= g_synth_state.chain_length) {
				g_synth_state.chain_position = 0;
			}

			// Update active pattern
			if (g_synth_state.chain_length > 0) {
				g_synth_state.active_pattern = g_synth_state.chain_order[g_synth_state.chain_position];
			} else {
				// No patterns chained, use current pattern
				g_synth_state.active_pattern = g_synth_state.current_pattern;
			}
		}
	}
}

bool synth_state_is_pattern_chained(int pattern_index)
{
	if (pattern_index >= 0 && pattern_index < PATTERN_COUNT) {
		return g_synth_state.pattern_chain[pattern_index];
	}
	return false;
}

void synth_state_clear_pattern_chain(void)
{
	for (int i = 0; i < PATTERN_COUNT; i++) {
		g_synth_state.pattern_chain[i] = false;
		g_synth_state.chain_order[i]   = -1;
	}
	g_synth_state.chain_length   = 0;
	g_synth_state.chain_position = 0;
	g_synth_state.active_pattern = g_synth_state.current_pattern;
}

int synth_state_get_active_pattern(void)
{
	return g_synth_state.active_pattern;
}
