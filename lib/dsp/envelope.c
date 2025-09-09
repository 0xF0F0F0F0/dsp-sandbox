#include "envelope.h"

void envelope_init(envelope_t* env, float sample_rate)
{
	env->level = 0.0f;
	env->state = ENV_IDLE;
	envelope_set_adsr(env, 10.0f, 100.0f, 0.7f, 200.0f);  // Default ADSR
}

void envelope_set_adsr(envelope_t* env, float attack_ms, float decay_ms, float sustain_level, float release_ms)
{
	float sample_rate  = 44100.0f;  // TODO: Pass sample rate properly
	env->attack_rate   = 1.0f / (attack_ms * 0.001f * sample_rate);
	env->decay_rate    = (1.0f - sustain_level) / (decay_ms * 0.001f * sample_rate);
	env->sustain_level = sustain_level;
	env->release_rate  = sustain_level / (release_ms * 0.001f * sample_rate);
}

void envelope_trigger(envelope_t* env)
{
	env->state = ENV_ATTACK;
}

void envelope_release(envelope_t* env)
{
	if (env->state != ENV_IDLE) {
		env->state = ENV_RELEASE;
	}
}

float envelope_process(envelope_t* env)
{
	switch (env->state) {
	case ENV_ATTACK:
		env->level += env->attack_rate;
		if (env->level >= 1.0f) {
			env->level = 1.0f;
			env->state = ENV_DECAY;
		}
		break;

	case ENV_DECAY:
		env->level -= env->decay_rate;
		if (env->level <= env->sustain_level) {
			env->level = env->sustain_level;
			env->state = ENV_SUSTAIN;
		}
		break;

	case ENV_SUSTAIN:
		// Hold at sustain level
		break;

	case ENV_RELEASE:
		env->level -= env->release_rate;
		if (env->level <= 0.0f) {
			env->level = 0.0f;
			env->state = ENV_IDLE;
		}
		break;

	case ENV_IDLE:
	default:
		env->level = 0.0f;
		break;
	}

	return env->level;
}

bool envelope_is_active(envelope_t* env)
{
	return env->state != ENV_IDLE;
}