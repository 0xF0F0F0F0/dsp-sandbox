#ifndef SCALES_H
#define SCALES_H

#include <stdbool.h>

// Step parameters for each step
typedef struct {
	int  note;    // 0-11 (C, C#, D, D#, E, F, F#, G, G#, A, A#, B)
	bool accent;  // on/off
	bool slide;   // on/off
} step_params_t;

// Scale functions
int	    get_scale_count(void);
const char* get_scale_name(int scale_index);
const int*  get_scale_intervals(int scale_index);
const char* get_note_name(int note_index);

#endif	// SCALES_H
