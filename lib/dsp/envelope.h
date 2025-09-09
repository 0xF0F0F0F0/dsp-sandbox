#ifndef ENVELOPE_H
#define ENVELOPE_H

#include <stdint.h>
#include <stdbool.h>

typedef enum { ENV_IDLE = 0, ENV_ATTACK, ENV_DECAY, ENV_SUSTAIN, ENV_RELEASE } envelope_state_t;

typedef struct {
	float		 level;
	envelope_state_t state;
	float		 attack_rate;
	float		 decay_rate;
	float		 sustain_level;
	float		 release_rate;
} envelope_t;

void  envelope_init(envelope_t* env, float sample_rate);
void  envelope_set_adsr(envelope_t* env, float attack_ms, float decay_ms, float sustain_level, float release_ms);
void  envelope_trigger(envelope_t* env);
void  envelope_release(envelope_t* env);
float envelope_process(envelope_t* env);
bool  envelope_is_active(envelope_t* env);

#endif  // ENVELOPE_H