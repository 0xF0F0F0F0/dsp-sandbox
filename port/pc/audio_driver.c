#include "audio_driver.h"
#include "audio_callback.h"
#include "portaudio.h"
#include <stdio.h>
#include <string.h>

// PortAudio stream handle
static PaStream* pa_stream	    = NULL;
static bool	 driver_initialized = false;
static bool	 driver_running	    = false;

// PortAudio callback function
static int pa_callback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer,
		       const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
{
	(void)inputBuffer;  // Unused
	(void)timeInfo;	    // Unused
	(void)statusFlags;  // Unused
	(void)userData;	    // Unused

	float* out = (float*)outputBuffer;

	// Call the platform-independent audio generation function
	audio_driver_callback(out, (uint32_t)framesPerBuffer);

	return paContinue;
}

bool audio_driver_init(void)
{
	if (driver_initialized) {
		return true;
	}

	// Initialize PortAudio
	PaError err = Pa_Initialize();
	if (err != paNoError) {
		fprintf(stderr, "PortAudio init failed: %s\n", Pa_GetErrorText(err));
		return false;
	}

	// Open default audio stream
	err = Pa_OpenDefaultStream(&pa_stream,
				   0,		       // no input channels
				   AUDIO_CHANNELS,     // stereo output
				   paFloat32,	       // 32-bit float samples
				   AUDIO_SAMPLE_RATE,  // sample rate
				   AUDIO_BUFFER_SIZE,  // frames per buffer
				   pa_callback,	       // callback function
				   NULL);	       // no user data

	if (err != paNoError) {
		fprintf(stderr, "PortAudio stream open failed: %s\n", Pa_GetErrorText(err));
		Pa_Terminate();
		return false;
	}

	driver_initialized = true;
	printf("Audio driver initialized: %d Hz, %d channels, %d frame buffer\n", AUDIO_SAMPLE_RATE, AUDIO_CHANNELS,
	       AUDIO_BUFFER_SIZE);

	return true;
}

void audio_driver_cleanup(void)
{
	if (!driver_initialized) {
		return;
	}

	// Stop stream if running
	if (driver_running) {
		audio_driver_stop();
	}

	// Close stream
	if (pa_stream) {
		Pa_CloseStream(pa_stream);
		pa_stream = NULL;
	}

	// Terminate PortAudio
	Pa_Terminate();

	driver_initialized = false;
	printf("Audio driver cleaned up\n");
}

bool audio_driver_start(void)
{
	if (!driver_initialized || !pa_stream) {
		fprintf(stderr, "Audio driver not initialized\n");
		return false;
	}

	if (driver_running) {
		return true;  // Already running
	}

	PaError err = Pa_StartStream(pa_stream);
	if (err != paNoError) {
		fprintf(stderr, "PortAudio stream start failed: %s\n", Pa_GetErrorText(err));
		return false;
	}

	driver_running = true;
	printf("Audio driver started\n");
	return true;
}

void audio_driver_stop(void)
{
	if (!driver_initialized || !pa_stream || !driver_running) {
		return;
	}

	PaError err = Pa_StopStream(pa_stream);
	if (err != paNoError) {
		fprintf(stderr, "PortAudio stream stop failed: %s\n", Pa_GetErrorText(err));
	}

	driver_running = false;
	printf("Audio driver stopped\n");
}

bool audio_driver_is_running(void)
{
	return driver_running;
}
