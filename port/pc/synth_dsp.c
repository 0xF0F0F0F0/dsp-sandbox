#include "synth_dsp.h"
#include "synth_state.h"
#include "oscillator.h"
#include "filter.h"
#include "envelope.h"
#include <math.h>

void synth_dsp_init(synth_dsp_state_t* dsp, float sample_rate)
{
	dsp->oscillator_phase  = 0.0f;
	dsp->current_frequency = 440.0f;
	dsp->filter_state_1    = 0.0f;
	dsp->filter_state_2    = 0.0f;
	dsp->envelope_level    = 0.0f;
	dsp->envelope_active   = false;

	// Initialize parameter smoothing with current synth state
	dsp->cutoff_current    = synth_state_get_parameter_normalized(PARAM_CUTOFF) * 8000.0f + 50.0f;
	dsp->resonance_current = synth_state_get_parameter_normalized(PARAM_RESONANCE) * 0.95f;
	dsp->volume_current    = synth_state_get_parameter_normalized(PARAM_VOLUME);
}

void synth_dsp_trigger_note(synth_dsp_state_t* dsp, float frequency)
{
	dsp->current_frequency = frequency;
	dsp->envelope_level    = 0.0f;
	dsp->envelope_active   = true;
}

void synth_dsp_trigger_note_off(synth_dsp_state_t* dsp)
{
	dsp->envelope_active = false;
}

static float process_oscillator(synth_dsp_state_t* dsp, float sample_rate)
{
	// Simple sine wave oscillator
	float output = sinf(2.0f * M_PI * dsp->oscillator_phase);
	dsp->oscillator_phase += dsp->current_frequency / sample_rate;
	if (dsp->oscillator_phase >= 1.0f) {
		dsp->oscillator_phase -= 1.0f;
	}
	return output;
}

static float process_envelope(synth_dsp_state_t* dsp, float sample_rate)
{
	const float attack_rate  = 1.0f / (0.01f * sample_rate);  // 10ms attack
	const float release_rate = 1.0f / (0.2f * sample_rate);   // 200ms release

	if (dsp->envelope_active) {
		// Attack
		dsp->envelope_level += attack_rate;
		if (dsp->envelope_level >= 1.0f) {
			dsp->envelope_level = 1.0f;
		}
	} else {
		// Release
		dsp->envelope_level -= release_rate;
		if (dsp->envelope_level <= 0.0f) {
			dsp->envelope_level = 0.0f;
		}
	}

	return dsp->envelope_level;
}

static float process_filter(synth_dsp_state_t* dsp, float input)
{
	// Simple 2-pole lowpass filter
	float cutoff    = dsp->cutoff_current / 22050.0f;  // Normalize to Nyquist
	float resonance = dsp->resonance_current * 4.0f;

	dsp->filter_state_1 +=
	    cutoff * (input - dsp->filter_state_1 + resonance * (dsp->filter_state_1 - dsp->filter_state_2));
	dsp->filter_state_2 += cutoff * (dsp->filter_state_1 - dsp->filter_state_2);

	return dsp->filter_state_2;
}

void synth_dsp_process(synth_dsp_state_t* dsp, float* output, uint32_t frames, float sample_rate)
{
	// Update parameter smoothing (simple approach)
	const float smoothing	= 0.99f;
	float       cutoff_target    = synth_state_get_parameter_normalized(PARAM_CUTOFF) * 8000.0f + 50.0f;
	float       resonance_target = synth_state_get_parameter_normalized(PARAM_RESONANCE) * 0.95f;
	float       volume_target    = synth_state_get_parameter_normalized(PARAM_VOLUME);

	dsp->cutoff_current    = dsp->cutoff_current * smoothing + cutoff_target * (1.0f - smoothing);
	dsp->resonance_current = dsp->resonance_current * smoothing + resonance_target * (1.0f - smoothing);
	dsp->volume_current    = dsp->volume_current * smoothing + volume_target * (1.0f - smoothing);

	// Process each frame through the DSP pipeline
	for (uint32_t i = 0; i < frames; i++) {
		// 1. Oscillator
		float osc_output = process_oscillator(dsp, sample_rate);

		// 2. Filter
		float filtered_output = process_filter(dsp, osc_output);

		// 3. Envelope
		float envelope = process_envelope(dsp, sample_rate);

		// 4. Final output with volume
		float final_output = filtered_output * envelope * dsp->volume_current * 0.3f;  // Scale down

		// Mono output (duplicate for stereo if needed by driver)
		output[i] = final_output;
	}
}