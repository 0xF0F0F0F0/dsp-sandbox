#include "oscillator.h"
#include <math.h>

void oscillator_init(oscillator_t* osc, float sample_rate)
{
	osc->phase       = 0.0f;
	osc->frequency   = 440.0f;
	osc->sample_rate = sample_rate;
}

void oscillator_set_frequency(oscillator_t* osc, float frequency)
{
	osc->frequency = frequency;
}

float oscillator_process(oscillator_t* osc)
{
	float output = sinf(2.0f * M_PI * osc->phase);

	osc->phase += osc->frequency / osc->sample_rate;
	if (osc->phase >= 1.0f) {
		osc->phase -= 1.0f;
	}

	return output;
}