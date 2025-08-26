#ifndef SYNTH_STATE_H
#define SYNTH_STATE_H

#include <stdbool.h>
#include <stdint.h>
#include "scales/scales.h"

// Sequencer configuration
#define SEQUENCER_STEPS 16

// Parameter indices for easy access
typedef enum {
	// Filter parameters (Tab 1)
	PARAM_CUTOFF = 0,
	PARAM_RESONANCE,
	PARAM_ENVELOPE,
	PARAM_ACCENT,
	
	// Envelope parameters (Tab 2)
	PARAM_ATTACK,
	PARAM_DECAY,
	PARAM_SUSTAIN,
	PARAM_RELEASE,
	
	// Effects parameters (Tab 3)
	PARAM_DISTORTION,
	PARAM_DELAY,
	PARAM_REVERB,
	PARAM_VOLUME,
	
	PARAM_COUNT
} synth_param_t;

// Sequencer step data
typedef struct {
	bool active;           // Step on/off (gate)
	int note_index;       // Index into current scale (0-7)
	bool accent;          // Accent on/off
	bool slide;           // Slide/glide on/off
} sequencer_step_t;

// Main synthesizer state
typedef struct {
	// Sequencer state
	sequencer_step_t steps[SEQUENCER_STEPS];
	int current_scale;                    // Index of current musical scale
	int current_step;                     // Currently playing step (for DSP)
	bool sequencer_running;               // Is sequencer playing?
	
	// Parameter values (0-100 range for UI, DSP can scale as needed)
	uint8_t parameters[PARAM_COUNT];
	
	// Timing/tempo
	float bpm;                            // Beats per minute
	uint32_t step_counter;                // Internal step counter for DSP
	
} synth_state_t;

// Global state instance (defined in synth_state.c)
extern synth_state_t g_synth_state;

// State management functions
void synth_state_init(void);
void synth_state_reset_sequence(void);
void synth_state_set_parameter(synth_param_t param, uint8_t value);
uint8_t synth_state_get_parameter(synth_param_t param);
void synth_state_set_step(int step_index, bool active, int note_index, bool accent, bool slide);
sequencer_step_t* synth_state_get_step(int step_index);
void synth_state_set_scale(int scale_index);
int synth_state_get_scale(void);

// DSP integration helpers
float synth_state_get_parameter_normalized(synth_param_t param);  // Returns 0.0-1.0
int synth_state_get_note_frequency(int step_index);               // Get actual note frequency for step
void synth_state_advance_step(void);                              // Called by DSP to advance sequencer

#endif // SYNTH_STATE_H