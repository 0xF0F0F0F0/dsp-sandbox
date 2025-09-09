#ifndef SEQUENCER_STEP_H
#define SEQUENCER_STEP_H

#include <stdbool.h>

typedef struct {
	bool active;      // Step on/off (gate)
	int  note_index;  // Index into current scale (0-7)
	int  octave;      // Octave offset (-2 to +2)
	bool accent;      // Accent on/off
	bool slide;       // Slide/glide on/off
} sequencer_step_t;

#endif  // SEQUENCER_STEP_H