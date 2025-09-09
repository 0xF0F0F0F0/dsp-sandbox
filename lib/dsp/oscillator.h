#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <stdint.h>

typedef struct {
	float phase;
	float frequency;
	float sample_rate;
} oscillator_t;

void  oscillator_init(oscillator_t* osc, float sample_rate);
void  oscillator_set_frequency(oscillator_t* osc, float frequency);
float oscillator_process(oscillator_t* osc);

#endif  // OSCILLATOR_H