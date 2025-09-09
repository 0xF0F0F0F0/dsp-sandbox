#ifndef SYNTH_DSP_H
#define SYNTH_DSP_H

#include <stdint.h>
#include <stdbool.h>

// DSP pipeline for synthesizer application
typedef struct {
	// Oscillator state
	float oscillator_phase;
	float current_frequency;

	// Filter state (2-pole lowpass)
	float filter_state_1;
	float filter_state_2;

	// Envelope state
	float envelope_level;
	bool  envelope_active;

	// Parameter smoothing
	float cutoff_current;
	float resonance_current;
	float volume_current;
} synth_dsp_state_t;

// Initialize DSP pipeline
void synth_dsp_init(synth_dsp_state_t* dsp, float sample_rate);

// Process audio through DSP pipeline
void synth_dsp_process(synth_dsp_state_t* dsp, float* output, uint32_t frames, float sample_rate);

// Trigger note on/off
void synth_dsp_trigger_note(synth_dsp_state_t* dsp, float frequency);
void synth_dsp_trigger_note_off(synth_dsp_state_t* dsp);

#endif  // SYNTH_DSP_H