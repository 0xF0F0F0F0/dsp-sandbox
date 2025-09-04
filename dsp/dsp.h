#ifndef DSP_H
#define DSP_H

#include <stdbool.h>
#include <stdint.h>

#define AUDIO_SAMPLE_RATE 44100

// Envelope states
typedef enum { ENV_IDLE = 0, ENV_ATTACK, ENV_DECAY, ENV_SUSTAIN, ENV_RELEASE } envelope_state_t;

// DSP processing state
typedef struct {
	// Oscillator state
	float oscillator_phase;
	float current_frequency;

	// Filter state (2-pole lowpass)
	float filter_state_1;
	float filter_state_2;

	// Envelope state
	float		 envelope_level;
	envelope_state_t envelope_state;

	// Sequencer timing
	uint32_t samples_per_step;
	uint32_t step_samples_remaining;

	// Parameter interpolation (for smooth parameter changes)
	float cutoff_target;
	float cutoff_current;
	float resonance_target;
	float resonance_current;
	float volume_target;
	float volume_current;
} dsp_state_t;

// DSP initialization and cleanup
void dsp_init(void);
void dsp_cleanup(void);

// Main DSP processing function
void dsp_process_audio(float* output, uint32_t frames);

// Control functions
void dsp_trigger_note_off(void);

#endif	// DSP_H
