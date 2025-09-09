#ifndef PATTERN_H
#define PATTERN_H

#include "step.h"
#include <stdbool.h>

#define SEQUENCER_STEPS 16
#define PATTERN_COUNT 4

typedef struct {
	sequencer_step_t steps[SEQUENCER_STEPS];
} pattern_t;

typedef struct {
	pattern_t patterns[PATTERN_COUNT];
	int       current_pattern;
	int       active_pattern;
	bool      pattern_chain[PATTERN_COUNT];
	int       chain_position;
	int       chain_length;
	int       chain_order[PATTERN_COUNT];
} pattern_manager_t;

void		  pattern_manager_init(pattern_manager_t* mgr);
void		  pattern_set_current(pattern_manager_t* mgr, int pattern_index);
int		  pattern_get_current(pattern_manager_t* mgr);
sequencer_step_t* pattern_get_step(pattern_manager_t* mgr, int pattern_index, int step_index);
sequencer_step_t* pattern_get_current_step(pattern_manager_t* mgr, int step_index);
void		  pattern_chain(pattern_manager_t* mgr, int pattern_index);
void		  pattern_unchain(pattern_manager_t* mgr, int pattern_index);
bool		  pattern_is_chained(pattern_manager_t* mgr, int pattern_index);
void		  pattern_clear_chain(pattern_manager_t* mgr);
bool		  pattern_advance_chain(pattern_manager_t* mgr);

#endif  // PATTERN_H