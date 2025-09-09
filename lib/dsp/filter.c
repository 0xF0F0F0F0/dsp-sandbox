#include "filter.h"

void filter_init(lowpass_filter_t* filter)
{
	filter->state1	  = 0.0f;
	filter->state2	  = 0.0f;
	filter->cutoff	  = 0.5f;
	filter->resonance = 0.1f;
}

void filter_set_params(lowpass_filter_t* filter, float cutoff, float resonance)
{
	filter->cutoff	  = cutoff;
	filter->resonance = resonance;
}

float filter_process(lowpass_filter_t* filter, float input)
{
	// Simple 2-pole lowpass filter
	float f = filter->cutoff;
	float q = filter->resonance * 4.0f;

	filter->state1 += f * (input - filter->state1 + q * (filter->state1 - filter->state2));
	filter->state2 += f * (filter->state1 - filter->state2);

	return filter->state2;
}
