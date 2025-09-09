#ifndef FILTER_H
#define FILTER_H

typedef struct {
	float state1;
	float state2;
	float cutoff;
	float resonance;
} lowpass_filter_t;

void  filter_init(lowpass_filter_t* filter);
void  filter_set_params(lowpass_filter_t* filter, float cutoff, float resonance);
float filter_process(lowpass_filter_t* filter, float input);

#endif  // FILTER_H