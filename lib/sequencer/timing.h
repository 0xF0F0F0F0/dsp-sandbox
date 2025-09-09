#ifndef TIMING_H
#define TIMING_H

#include <stdint.h>

typedef struct {
	float    bpm;
	uint32_t samples_per_step;
	uint32_t step_samples_remaining;
	int      current_step;
} timing_t;

void     timing_init(timing_t* timing, float sample_rate);
void     timing_set_bpm(timing_t* timing, float bpm, float sample_rate);
void     timing_reset(timing_t* timing);
int      timing_advance(timing_t* timing, uint32_t samples);
int      timing_get_current_step(timing_t* timing);
uint32_t timing_get_samples_per_step(timing_t* timing);

#endif  // TIMING_H