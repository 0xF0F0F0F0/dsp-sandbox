#include "dsp.h"
#include "synth_state/synth_state.h"
#include "scales/scales.h"
#include <arm_math.h>
#include <stdio.h>
#include <stdlib.h>

// DSP state
static dsp_state_t dsp_state;
static bool	   dsp_initialized = false;

// Lookup table for note frequencies (C4 = 261.63 Hz)
static const float note_frequencies[12] = {
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

void dsp_init(void)
{
	if (dsp_initialized) {
		return;
	}

	// Initialize DSP state
	dsp_state.oscillator_phase = 0.0f;
	dsp_state.filter_state_1   = 0.0f;
	dsp_state.filter_state_2   = 0.0f;
	dsp_state.envelope_level   = 0.0f;
	dsp_state.envelope_state   = ENV_IDLE;

	// Initialize sequencer timing
	dsp_state.step_samples_remaining = 0;
	dsp_state.samples_per_step	 = AUDIO_SAMPLE_RATE * 60.0f / (g_synth_state.bpm * 4.0f);  // 16th notes

	// Initialize parameter interpolation
	float initial_cutoff	= synth_state_get_parameter_normalized(PARAM_CUTOFF) * 8000.0f + 50.0f;
	float initial_resonance = synth_state_get_parameter_normalized(PARAM_RESONANCE) * 0.95f;
	float initial_volume	= synth_state_get_parameter_normalized(PARAM_VOLUME);

	dsp_state.cutoff_target = dsp_state.cutoff_current = initial_cutoff;
	dsp_state.resonance_target = dsp_state.resonance_current = initial_resonance;
	dsp_state.volume_target = dsp_state.volume_current = initial_volume;

	dsp_initialized = true;
	printf("DSP initialized at %d Hz\n", AUDIO_SAMPLE_RATE);
}

void dsp_cleanup(void)
{
	dsp_initialized = false;
}

static float calculate_note_frequency(int step_index)
{
	// Use active pattern (currently playing) for DSP
	sequencer_step_t* step = synth_state_get_step_from_pattern(synth_state_get_active_pattern(), step_index);
	if (!step || !step->active) {
		return 0.0f;
	}

	const int* scale_intervals = get_scale_intervals(synth_state_get_scale());
	if (!scale_intervals || step->note_index < 0 || step->note_index >= 8) {
		return note_frequencies[0];  // Default to C4
	}

	int   note	= scale_intervals[step->note_index] % 12;
	float base_freq = note_frequencies[note];

	// Apply octave offset: each octave doubles/halves the frequency
	for (int i = 0; i < abs(step->octave); i++) {
		if (step->octave > 0) {
			base_freq *= 2.0f;  // Octave up
		} else if (step->octave < 0) {
			base_freq /= 2.0f;  // Octave down
		}
	}

	return base_freq;
}

static void update_sequencer_timing(void)
{
	// Calculate samples per step based on current BPM
	float new_samples_per_step = AUDIO_SAMPLE_RATE * 60.0f / (g_synth_state.bpm * 4.0f);
	dsp_state.samples_per_step = (uint32_t)new_samples_per_step;
}

static void process_sequencer_step(void)
{
	static bool sequencer_just_started = false;

	if (!g_synth_state.sequencer_running) {
		// When sequencer stops, immediately go to release state
		if (dsp_state.envelope_state != ENV_IDLE && dsp_state.envelope_state != ENV_RELEASE) {
			dsp_state.envelope_state = ENV_RELEASE;
		}
		sequencer_just_started = true;	// Flag for next start
		return;
	}

	// Check if we need to advance to next step OR if sequencer just started
	if (dsp_state.step_samples_remaining <= 0 || sequencer_just_started) {
		// Only advance step if not just starting (we want to start at step 0)
		if (!sequencer_just_started) {
			synth_state_advance_step();
		}
		sequencer_just_started = false;	 // Clear the flag

		// Reset sample counter for new step
		dsp_state.step_samples_remaining = dsp_state.samples_per_step;

		// Update oscillator frequency for new step
		float freq = calculate_note_frequency(g_synth_state.current_step);
		if (freq > 0.0f) {
			dsp_state.current_frequency = freq;

			// Trigger envelope if step is active (use active pattern)
			sequencer_step_t* step = synth_state_get_step_from_pattern(synth_state_get_active_pattern(),
										   g_synth_state.current_step);
			if (step && step->active) {
				dsp_state.envelope_state = ENV_ATTACK;
				dsp_state.envelope_level = 0.0f;
			}
		} else {
			dsp_state.current_frequency = 0.0f;
			dsp_state.envelope_state    = ENV_IDLE;
		}
	}
}

static float process_oscillator(float frequency)
{
	if (frequency <= 0.0f) {
		return 0.0f;
	}

	// Calculate phase increment
	float phase_increment = 2.0f * PI * frequency / AUDIO_SAMPLE_RATE;

	// Generate sawtooth wave using CMSIS-DSP
	float sample = (dsp_state.oscillator_phase / PI) - 1.0f;  // Convert to -1 to 1 range

	// Update phase
	dsp_state.oscillator_phase += phase_increment;
	if (dsp_state.oscillator_phase >= 2.0f * PI) {
		dsp_state.oscillator_phase -= 2.0f * PI;
	}

	return sample;
}

static float process_filter(float input, float cutoff, float resonance)
{
	// Simple 2-pole lowpass filter (Chamberlin SVF)
	float f = 2.0f * arm_sin_f32(PI * cutoff / AUDIO_SAMPLE_RATE);
	float q = 1.0f - resonance;

	// Clamp to prevent instability
	if (f > 1.0f)
		f = 1.0f;
	if (q < 0.1f)
		q = 0.1f;

	// Filter processing
	dsp_state.filter_state_1 += f * (input - dsp_state.filter_state_1 - q * dsp_state.filter_state_2);
	dsp_state.filter_state_2 += f * dsp_state.filter_state_1;

	return dsp_state.filter_state_2;
}

static float process_envelope(void)
{
	// Get normalized parameter values (0.0 to 1.0)
	float attack  = synth_state_get_parameter_normalized(PARAM_ATTACK) * 2.0f + 0.001f;   // 1ms to 2s
	float decay   = synth_state_get_parameter_normalized(PARAM_DECAY) * 2.0f + 0.001f;    // 1ms to 2s
	float sustain = synth_state_get_parameter_normalized(PARAM_SUSTAIN);		      // 0.0 to 1.0
	float release = synth_state_get_parameter_normalized(PARAM_RELEASE) * 3.0f + 0.001f;  // 1ms to 3s

	float envelope_increment = 1.0f / AUDIO_SAMPLE_RATE;

	switch (dsp_state.envelope_state) {
	case ENV_ATTACK:
		dsp_state.envelope_level += envelope_increment / attack;
		if (dsp_state.envelope_level >= 1.0f) {
			dsp_state.envelope_level = 1.0f;
			dsp_state.envelope_state = ENV_DECAY;
		}
		break;

	case ENV_DECAY:
		dsp_state.envelope_level -= envelope_increment * (1.0f - sustain) / decay;
		if (dsp_state.envelope_level <= sustain) {
			dsp_state.envelope_level = sustain;
			dsp_state.envelope_state = ENV_SUSTAIN;
		}
		break;

	case ENV_SUSTAIN:
		dsp_state.envelope_level = sustain;
		break;

	case ENV_RELEASE:
		dsp_state.envelope_level -= envelope_increment / release;
		if (dsp_state.envelope_level <= 0.0f) {
			dsp_state.envelope_level = 0.0f;
			dsp_state.envelope_state = ENV_IDLE;
		}
		break;

	case ENV_IDLE:
	default:
		dsp_state.envelope_level = 0.0f;
		break;
	}

	return dsp_state.envelope_level;
}

void dsp_process_audio(float* output, uint32_t frames)
{
	if (!dsp_initialized) {
		// Output silence if not initialized
		for (uint32_t i = 0; i < frames * 2; i++) {
			output[i] = 0.0f;
		}
		return;
	}

	// Update sequencer timing in case BPM changed
	update_sequencer_timing();

	// Update target parameter values
	dsp_state.cutoff_target =
		synth_state_get_parameter_normalized(PARAM_CUTOFF) * 8000.0f + 50.0f;		     // 50 Hz to 8 kHz
	dsp_state.resonance_target = synth_state_get_parameter_normalized(PARAM_RESONANCE) * 0.95f;  // 0 to 0.95
	dsp_state.volume_target	   = synth_state_get_parameter_normalized(PARAM_VOLUME);

	// Interpolation coefficients (adjust for desired smoothness)
	const float interp_coeff = 0.001f;  // Smaller = smoother, larger = more responsive

	for (uint32_t frame = 0; frame < frames; frame++) {
		// Interpolate parameters smoothly
		dsp_state.cutoff_current += (dsp_state.cutoff_target - dsp_state.cutoff_current) * interp_coeff;
		dsp_state.resonance_current +=
			(dsp_state.resonance_target - dsp_state.resonance_current) * interp_coeff;
		dsp_state.volume_current += (dsp_state.volume_target - dsp_state.volume_current) * interp_coeff;

		// Process sequencer timing
		process_sequencer_step();

		// Generate oscillator sample
		float osc_sample = process_oscillator(dsp_state.current_frequency);

		// Apply filter with interpolated parameters
		float filtered_sample =
			process_filter(osc_sample, dsp_state.cutoff_current, dsp_state.resonance_current);

		// Apply envelope
		float envelope = process_envelope();
		float final_sample =
			filtered_sample * envelope * dsp_state.volume_current * 0.3f;  // Scale down to prevent clipping

		// Output stereo (same signal to both channels)
		output[frame * 2]     = final_sample;  // Left
		output[frame * 2 + 1] = final_sample;  // Right

		// Decrement step sample counter
		if (dsp_state.step_samples_remaining > 0) {
			dsp_state.step_samples_remaining--;
		}
	}
}

void dsp_trigger_note_off(void)
{
	if (dsp_state.envelope_state != ENV_IDLE) {
		dsp_state.envelope_state = ENV_RELEASE;
	}
}
