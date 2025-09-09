#include "timing.h"
#include "pattern.h"

void timing_init(timing_t* timing, float sample_rate)
{
	timing->bpm	  = 120.0f;
	timing->current_step = 0;
	timing_set_bpm(timing, timing->bpm, sample_rate);
}

void timing_set_bpm(timing_t* timing, float bpm, float sample_rate)
{
	timing->bpm = bpm;
	// 16th notes at given BPM
	timing->samples_per_step       = (uint32_t)(sample_rate * 60.0f / (bpm * 4.0f));
	timing->step_samples_remaining = timing->samples_per_step;
}

void timing_reset(timing_t* timing)
{
	timing->current_step	   = 0;
	timing->step_samples_remaining = timing->samples_per_step;
}

int timing_advance(timing_t* timing, uint32_t samples)
{
	int steps_advanced = 0;

	if (timing->step_samples_remaining <= samples) {
		// Step boundary crossed
		samples -= timing->step_samples_remaining;
		steps_advanced++;
		timing->current_step	   = (timing->current_step + 1) % SEQUENCER_STEPS;
		timing->step_samples_remaining = timing->samples_per_step;

		// Handle case where multiple steps are crossed in one buffer
		while (samples >= timing->samples_per_step) {
			samples -= timing->samples_per_step;
			steps_advanced++;
			timing->current_step = (timing->current_step + 1) % SEQUENCER_STEPS;
		}

		timing->step_samples_remaining -= samples;
	} else {
		timing->step_samples_remaining -= samples;
	}

	return steps_advanced;
}

int timing_get_current_step(timing_t* timing)
{
	return timing->current_step;
}

uint32_t timing_get_samples_per_step(timing_t* timing)
{
	return timing->samples_per_step;
}