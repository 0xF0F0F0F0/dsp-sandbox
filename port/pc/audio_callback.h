#ifndef AUDIO_CALLBACK_H
#define AUDIO_CALLBACK_H

#include <stdint.h>

// Platform-independent audio generation callback
// This function is called by platform-specific audio drivers
void audio_driver_callback(float* out, uint32_t frames);

#endif	// AUDIO_CALLBACK_H
