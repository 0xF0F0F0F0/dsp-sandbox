#include "audio_callback.h"
#include "synth_dsp.h"
#include "synth_state.h"
#include "notes.h"
#include "constants.h"

// Static DSP state for this audio callback
static synth_dsp_state_t dsp_state;
static bool		 dsp_initialized      = false;
static bool		 prev_sequencer_running = false;
static int		 prev_step_index      = -1;

void audio_driver_callback(float* out, uint32_t frames)
{
	const float sample_rate = (float)AUDIO_SAMPLE_RATE;

	// Initialize DSP on first call
	if (!dsp_initialized) {
		synth_dsp_init(&dsp_state, sample_rate);
		dsp_initialized = true;
	}

	// Check current sequencer state
	bool is_sequencer_running = synth_state_is_running();

	// Detect sequencer transitions
	if (prev_sequencer_running && !is_sequencer_running) {
		// Sequencer just stopped - immediately trigger note off
		synth_dsp_trigger_note_off(&dsp_state);
	} else if (!prev_sequencer_running && is_sequencer_running) {
		// Sequencer just started - reset step tracking and trigger first step
		prev_step_index = -1;
		sequencer_step_t* step = synth_state_get_current_active_step();
		if (step && step->active) {
			int   active_pattern = synth_state_get_active_pattern();
			int   current_step   = synth_state_get_current_step_index();
			float frequency = synth_state_get_note_frequency_from_pattern(active_pattern, current_step);
			if (frequency > 0) {
				synth_dsp_trigger_note(&dsp_state, frequency);
			}
		}
		// Also process timing for this frame
		synth_state_process_step(frames);
	} else if (is_sequencer_running) {
		// Sequencer is running - process normal step logic
		bool step_changed = synth_state_process_step(frames);

		if (step_changed) {
			int current_step = synth_state_get_current_step_index();
			
			// Check for pattern completion (step wrapped from 15 to 0)
			if (prev_step_index == 15 && current_step == 0) {
				// Pattern completed - try to advance chain
				synth_state_advance_pattern_chain();
			}
			prev_step_index = current_step;
			
			// New step - check if we should trigger a note
			sequencer_step_t* step = synth_state_get_current_active_step();
			if (step && step->active) {
				// Calculate frequency for this step from the active pattern
				int   active_pattern = synth_state_get_active_pattern();
				int   current_step   = synth_state_get_current_step_index();
				float frequency = synth_state_get_note_frequency_from_pattern(active_pattern, current_step);
				if (frequency > 0) {
					synth_dsp_trigger_note(&dsp_state, frequency);
				}
			} else {
				// No active step - trigger note off
				synth_dsp_trigger_note_off(&dsp_state);
			}
		}
	}

	// Update previous state for next callback
	prev_sequencer_running = is_sequencer_running;

	// Process audio through DSP pipeline
	synth_dsp_process(&dsp_state, out, frames, sample_rate);
}
