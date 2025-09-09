#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <stdint.h>

// Parameter indices for synthesis parameters
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

typedef struct {
	uint8_t values[PARAM_COUNT];
} parameter_manager_t;

void    parameter_manager_init(parameter_manager_t* mgr);
void    parameter_set(parameter_manager_t* mgr, synth_param_t param, uint8_t value);
uint8_t parameter_get(parameter_manager_t* mgr, synth_param_t param);
float   parameter_get_normalized(parameter_manager_t* mgr, synth_param_t param);

#endif  // PARAMETERS_H