#include "audio_callback.h"
#include "dsp.h"

void audio_driver_callback(float* out, uint32_t frames)
{
    // This is the platform-independent audio generation function
    // called by platform-specific audio drivers (PC PortAudio, STM32 I2S, etc.)
    dsp_process_audio(out, frames);
}