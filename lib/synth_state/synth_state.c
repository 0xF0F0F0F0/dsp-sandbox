#include "synth_state.h"
#include <string.h>

// Global synthesizer state
synth_state_t g_synth_state;

// Note frequencies in Hz (C4 to B4, can be scaled for other octaves)
static const float note_frequencies[] = {
	261.63f, // C4
	277.18f, // C#4
	293.66f, // D4
	311.13f, // D#4
	329.63f, // E4
	349.23f, // F4
	369.99f, // F#4
	392.00f, // G4
	415.30f, // G#4
	440.00f, // A4
	466.16f, // A#4
	493.88f  // B4
};

void synth_state_init(void)
{
	// Clear all state
	memset(&g_synth_state, 0, sizeof(synth_state_t));
	
	// Set default values
	g_synth_state.current_scale = 0;  // Major scale
	g_synth_state.current_step = 0;
	g_synth_state.sequencer_running = false;
	g_synth_state.bpm = 120.0f;
	g_synth_state.step_counter = 0;
	
	// Set default parameter values (50% for most)
	g_synth_state.parameters[PARAM_CUTOFF] = 50;
	g_synth_state.parameters[PARAM_RESONANCE] = 50;
	g_synth_state.parameters[PARAM_ENVELOPE] = 50;
	g_synth_state.parameters[PARAM_ACCENT] = 50;
	g_synth_state.parameters[PARAM_ATTACK] = 50;
	g_synth_state.parameters[PARAM_DECAY] = 50;
	g_synth_state.parameters[PARAM_SUSTAIN] = 50;
	g_synth_state.parameters[PARAM_RELEASE] = 50;
	g_synth_state.parameters[PARAM_DISTORTION] = 50;
	g_synth_state.parameters[PARAM_DELAY] = 50;
	g_synth_state.parameters[PARAM_REVERB] = 50;
	g_synth_state.parameters[PARAM_VOLUME] = 80;  // Default volume higher
}

void synth_state_reset_sequence(void)
{
	// Clear all sequencer steps
	for (int i = 0; i < SEQUENCER_STEPS; i++) {
		g_synth_state.steps[i].active = false;
		g_synth_state.steps[i].note_index = 0;
		g_synth_state.steps[i].accent = false;
		g_synth_state.steps[i].slide = false;
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
		g_synth_state.steps[step_index].active = active;
		g_synth_state.steps[step_index].note_index = note_index;
		g_synth_state.steps[step_index].accent = accent;
		g_synth_state.steps[step_index].slide = slide;
	}
}

sequencer_step_t* synth_state_get_step(int step_index)
{
	if (step_index >= 0 && step_index < SEQUENCER_STEPS) {
		return &g_synth_state.steps[step_index];
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
	
	sequencer_step_t* step = &g_synth_state.steps[step_index];
	if (!step->active) {
		return 0;  // Step is off
	}
	
	// Get the note from the current scale
	const int* intervals = get_scale_intervals(g_synth_state.current_scale);
	if (step->note_index >= 0 && step->note_index < 8 && intervals[step->note_index] >= 0) {
		int note_index = intervals[step->note_index] % 12;
		return (int)note_frequencies[note_index];
	}
	
	return (int)note_frequencies[0];  // Default to C4
}

void synth_state_advance_step(void)
{
	g_synth_state.step_counter++;
	g_synth_state.current_step = (g_synth_state.current_step + 1) % SEQUENCER_STEPS;
}