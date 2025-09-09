#ifndef SYNTH_STATE_H
#define SYNTH_STATE_H

#include "pattern.h"
#include "parameters.h"
#include "timing.h"
#include <stdbool.h>

typedef struct {
	pattern_manager_t   patterns;
	parameter_manager_t parameters;
	timing_t	    timing;
	int		    current_scale;
	bool		    sequencer_running;
} synth_state_t;

// Global state instance
extern synth_state_t g_synth_state;

// State management functions
void synth_state_init(void);
void synth_state_reset_sequence(void);

// Parameter management wrapper functions
void    synth_state_set_parameter(synth_param_t param, uint8_t value);
uint8_t synth_state_get_parameter(synth_param_t param);
float   synth_state_get_parameter_normalized(synth_param_t param);

// Step management wrapper functions
void		  synth_state_set_step(int step_index, bool active, int note_index, bool accent, bool slide);
sequencer_step_t* synth_state_get_step(int step_index);

// Scale management
void synth_state_set_scale(int scale_index);
int  synth_state_get_scale(void);

// Pattern management wrapper functions
void		  synth_state_set_current_pattern(int pattern_index);
int		  synth_state_get_current_pattern(void);
sequencer_step_t* synth_state_get_step_from_pattern(int pattern_index, int step_index);
sequencer_step_t* synth_state_get_current_step(int step_index);
void		  synth_state_chain_pattern(int pattern_index);
void		  synth_state_unchain_pattern(int pattern_index);
bool		  synth_state_is_pattern_chained(int pattern_index);
void		  synth_state_clear_pattern_chain(void);
int		  synth_state_get_active_pattern(void);
bool		  synth_state_advance_pattern_chain(void);

// Timing wrapper functions
void  synth_state_set_bpm(float bpm, float sample_rate);
float synth_state_get_bpm(void);
int   synth_state_advance_step(uint32_t samples);
int   synth_state_get_current_step_index(void);

// Sequencer control
void synth_state_start_sequencer(void);
void synth_state_stop_sequencer(void);
bool synth_state_is_running(void);

// Sequencer step processing (for audio callbacks)
bool		  synth_state_process_step(uint32_t samples_this_frame);
sequencer_step_t* synth_state_get_current_active_step(void);

// DSP integration helpers
int synth_state_get_note_frequency(int step_index);
float synth_state_get_note_frequency_from_pattern(int pattern_index, int step_index);

#endif  // SYNTH_STATE_H