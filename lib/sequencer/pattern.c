#include "pattern.h"
#include <string.h>

void pattern_manager_init(pattern_manager_t* mgr)
{
	memset(mgr, 0, sizeof(pattern_manager_t));
	mgr->current_pattern = 0;
	mgr->active_pattern  = 0;
}

void pattern_set_current(pattern_manager_t* mgr, int pattern_index)
{
	if (pattern_index >= 0 && pattern_index < PATTERN_COUNT) {
		mgr->current_pattern = pattern_index;
	}
}

int pattern_get_current(pattern_manager_t* mgr)
{
	return mgr->current_pattern;
}

sequencer_step_t* pattern_get_step(pattern_manager_t* mgr, int pattern_index, int step_index)
{
	if (pattern_index < 0 || pattern_index >= PATTERN_COUNT || step_index < 0 || step_index >= SEQUENCER_STEPS) {
		return NULL;
	}
	return &mgr->patterns[pattern_index].steps[step_index];
}

sequencer_step_t* pattern_get_current_step(pattern_manager_t* mgr, int step_index)
{
	return pattern_get_step(mgr, mgr->current_pattern, step_index);
}

void pattern_chain(pattern_manager_t* mgr, int pattern_index)
{
	if (pattern_index >= 0 && pattern_index < PATTERN_COUNT) {
		mgr->pattern_chain[pattern_index] = true;
	}
}

void pattern_unchain(pattern_manager_t* mgr, int pattern_index)
{
	if (pattern_index >= 0 && pattern_index < PATTERN_COUNT) {
		mgr->pattern_chain[pattern_index] = false;
	}
}

bool pattern_is_chained(pattern_manager_t* mgr, int pattern_index)
{
	if (pattern_index >= 0 && pattern_index < PATTERN_COUNT) {
		return mgr->pattern_chain[pattern_index];
	}
	return false;
}

void pattern_clear_chain(pattern_manager_t* mgr)
{
	memset(mgr->pattern_chain, 0, sizeof(mgr->pattern_chain));
	mgr->chain_position = 0;
	mgr->chain_length   = 0;
}

static void pattern_build_chain_order(pattern_manager_t* mgr)
{
	mgr->chain_length = 0;
	// Build chain order based on pattern indices that are chained
	for (int i = 0; i < PATTERN_COUNT; i++) {
		if (mgr->pattern_chain[i]) {
			mgr->chain_order[mgr->chain_length] = i;
			mgr->chain_length++;
		}
	}
}

bool pattern_advance_chain(pattern_manager_t* mgr)
{
	// Rebuild chain order in case it changed
	pattern_build_chain_order(mgr);
	
	if (mgr->chain_length <= 1) {
		// No chain or single pattern - no advance needed
		return false;
	}
	
	// Find current active pattern position in chain
	int current_position = -1;
	if (mgr->active_pattern >= 0) {
		for (int i = 0; i < mgr->chain_length; i++) {
			if (mgr->chain_order[i] == mgr->active_pattern) {
				current_position = i;
				break;
			}
		}
	}
	
	if (current_position >= 0) {
		// Advance to next pattern in chain
		mgr->chain_position = (current_position + 1) % mgr->chain_length;
		mgr->active_pattern = mgr->chain_order[mgr->chain_position];
		return true;
	}
	
	// Current pattern not in chain - start from beginning of chain
	mgr->chain_position = 0;
	mgr->active_pattern = mgr->chain_order[0];
	return true;
}