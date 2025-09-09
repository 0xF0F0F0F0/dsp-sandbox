#include "synth_state.h"
#include "scales.h"
#include "notes.h"
#include <stddef.h>

// Global state instance
synth_state_t g_synth_state;

void synth_state_init(void)
{
	pattern_manager_init(&g_synth_state.patterns);
	parameter_manager_init(&g_synth_state.parameters);
	timing_init(&g_synth_state.timing, 44100.0f);  // Default sample rate
	g_synth_state.current_scale     = 0;	   // Major scale
	g_synth_state.sequencer_running = false;
	
	// Ensure pattern 1 (index 0) is chained by default
	synth_state_chain_pattern(0);
}

void synth_state_reset_sequence(void)
{
	timing_reset(&g_synth_state.timing);
	g_synth_state.sequencer_running = false;
}

void synth_state_set_parameter(synth_param_t param, uint8_t value)
{
	parameter_set(&g_synth_state.parameters, param, value);
}

uint8_t synth_state_get_parameter(synth_param_t param)
{
	return parameter_get(&g_synth_state.parameters, param);
}

float synth_state_get_parameter_normalized(synth_param_t param)
{
	return parameter_get_normalized(&g_synth_state.parameters, param);
}

void synth_state_set_step(int step_index, bool active, int note_index, bool accent, bool slide)
{
	sequencer_step_t* step = pattern_get_current_step(&g_synth_state.patterns, step_index);
	if (step) {
		step->active     = active;
		step->note_index = note_index;
		step->accent     = accent;
		step->slide      = slide;
	}
}

sequencer_step_t* synth_state_get_step(int step_index)
{
	return pattern_get_current_step(&g_synth_state.patterns, step_index);
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

void synth_state_set_current_pattern(int pattern_index)
{
	pattern_set_current(&g_synth_state.patterns, pattern_index);
}

int synth_state_get_current_pattern(void)
{
	return pattern_get_current(&g_synth_state.patterns);
}

sequencer_step_t* synth_state_get_step_from_pattern(int pattern_index, int step_index)
{
	return pattern_get_step(&g_synth_state.patterns, pattern_index, step_index);
}

sequencer_step_t* synth_state_get_current_step(int step_index)
{
	return pattern_get_current_step(&g_synth_state.patterns, step_index);
}

void synth_state_chain_pattern(int pattern_index)
{
	pattern_chain(&g_synth_state.patterns, pattern_index);
}

void synth_state_unchain_pattern(int pattern_index)
{
	pattern_unchain(&g_synth_state.patterns, pattern_index);
}

bool synth_state_is_pattern_chained(int pattern_index)
{
	return pattern_is_chained(&g_synth_state.patterns, pattern_index);
}

void synth_state_clear_pattern_chain(void)
{
	pattern_clear_chain(&g_synth_state.patterns);
}

int synth_state_get_active_pattern(void)
{
	return g_synth_state.patterns.active_pattern;
}

bool synth_state_advance_pattern_chain(void)
{
	return pattern_advance_chain(&g_synth_state.patterns);
}

void synth_state_set_bpm(float bpm, float sample_rate)
{
	timing_set_bpm(&g_synth_state.timing, bpm, sample_rate);
}

int synth_state_advance_step(uint32_t samples)
{
	return timing_advance(&g_synth_state.timing, samples);
}

int synth_state_get_current_step_index(void)
{
	return timing_get_current_step(&g_synth_state.timing);
}

float synth_state_get_bpm(void)
{
	return g_synth_state.timing.bpm;
}

void synth_state_start_sequencer(void)
{
	g_synth_state.sequencer_running = true;
	timing_reset(&g_synth_state.timing);
	
	// Find first chained pattern to start with, or use pattern 0 if no patterns are chained
	bool found_chained = false;
	for (int i = 0; i < PATTERN_COUNT; i++) {
		if (synth_state_is_pattern_chained(i)) {
			g_synth_state.patterns.active_pattern = i;
			found_chained = true;
			break;
		}
	}
	
	// If no patterns are chained, set active pattern to -1 (no pattern playing)
	if (!found_chained) {
		g_synth_state.patterns.active_pattern = -1;
	}
}

void synth_state_stop_sequencer(void)
{
	g_synth_state.sequencer_running = false;
}

bool synth_state_is_running(void)
{
	return g_synth_state.sequencer_running;
}

bool synth_state_process_step(uint32_t samples_this_frame)
{
	if (!g_synth_state.sequencer_running) {
		return false;
	}

	// Process timing and advance steps as needed
	int steps_advanced = timing_advance(&g_synth_state.timing, samples_this_frame);
	return steps_advanced > 0;
}

sequencer_step_t* synth_state_get_current_active_step(void)
{
	if (!g_synth_state.sequencer_running) {
		return NULL;
	}

	int active_pattern = g_synth_state.patterns.active_pattern;
	
	// If no pattern is active (no chained patterns), return NULL
	if (active_pattern < 0) {
		return NULL;
	}

	int current_step = timing_get_current_step(&g_synth_state.timing);
	return pattern_get_step(&g_synth_state.patterns, active_pattern, current_step);
}

int synth_state_get_note_frequency(int step_index)
{
	sequencer_step_t* step = synth_state_get_step(step_index);
	if (!step || !step->active) {
		return 0;
	}

	const int* scale_intervals = get_scale_intervals(g_synth_state.current_scale);
	if (!scale_intervals || step->note_index < 0 || step->note_index >= 8) {
		return 0;
	}

	int note   = scale_intervals[step->note_index] % 12;
	int octave = 4 + step->octave;  // Base octave 4

	return (int)note_name_to_frequency(note, octave);
}

float synth_state_get_note_frequency_from_pattern(int pattern_index, int step_index)
{
	sequencer_step_t* step = synth_state_get_step_from_pattern(pattern_index, step_index);
	if (!step || !step->active) {
		return 0.0f;
	}

	const int* scale_intervals = get_scale_intervals(g_synth_state.current_scale);
	if (!scale_intervals || step->note_index < 0 || step->note_index >= 8) {
		return 0.0f;
	}

	int note   = scale_intervals[step->note_index] % 12;
	int octave = 4 + step->octave;  // Base octave 4

	return note_name_to_frequency(note, octave);
}