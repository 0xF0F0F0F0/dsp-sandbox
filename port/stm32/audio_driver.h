#ifndef AUDIO_DRIVER_H
#define AUDIO_DRIVER_H

#include <stdbool.h>
#include <stdint.h>

#define AUDIO_SAMPLE_RATE 44100
#define AUDIO_BUFFER_SIZE 256
#define AUDIO_CHANNELS 2

// Audio driver initialization and cleanup
bool audio_driver_init(void);
void audio_driver_cleanup(void);

// Audio driver control
bool audio_driver_start(void);
void audio_driver_stop(void);
bool audio_driver_is_running(void);

// Audio generation callback - to be implemented by platform-specific code
// This function will be called by the audio driver to fill audio buffers
// out: stereo interleaved float samples (L, R, L, R, ...)
// frames: number of stereo frames to generate
void audio_driver_callback(float* out, uint32_t frames);

#endif  // AUDIO_DRIVER_H
