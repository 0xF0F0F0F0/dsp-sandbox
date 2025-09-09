#include "parameters.h"
#include <string.h>

void parameter_manager_init(parameter_manager_t* mgr)
{
	memset(mgr->values, 0, sizeof(mgr->values));

	// Set some reasonable defaults
	mgr->values[PARAM_CUTOFF]    = 80;
	mgr->values[PARAM_RESONANCE] = 20;
	mgr->values[PARAM_ENVELOPE]  = 50;
	mgr->values[PARAM_ACCENT]    = 70;
	mgr->values[PARAM_ATTACK]    = 5;
	mgr->values[PARAM_DECAY]     = 30;
	mgr->values[PARAM_SUSTAIN]   = 70;
	mgr->values[PARAM_RELEASE]   = 40;
	mgr->values[PARAM_VOLUME]    = 80;
}

void parameter_set(parameter_manager_t* mgr, synth_param_t param, uint8_t value)
{
	if (param < PARAM_COUNT) {
		mgr->values[param] = value;
	}
}

uint8_t parameter_get(parameter_manager_t* mgr, synth_param_t param)
{
	if (param < PARAM_COUNT) {
		return mgr->values[param];
	}
	return 0;
}

float parameter_get_normalized(parameter_manager_t* mgr, synth_param_t param)
{
	return parameter_get(mgr, param) / 100.0f;
}