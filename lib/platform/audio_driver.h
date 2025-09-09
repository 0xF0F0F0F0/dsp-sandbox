#ifndef PLATFORM_AUDIO_DRIVER_H
#define PLATFORM_AUDIO_DRIVER_H

#include <stdbool.h>
#include <stdint.h>

// Audio callback function signature
typedef void (*audio_callback_t)(float* output, uint32_t frames);

// Platform-agnostic audio driver interface
bool audio_driver_init(void);
bool audio_driver_start(void);
void audio_driver_stop(void);
void audio_driver_cleanup(void);
void audio_driver_set_callback(audio_callback_t callback);

#endif  // PLATFORM_AUDIO_DRIVER_H
