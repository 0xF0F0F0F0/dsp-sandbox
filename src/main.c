#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "ui.h"
#include "ui_backend.h"
#include "portaudio.h"
#include <SDL2/SDL.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define SAMPLE_RATE 44100
#define FREQ 200.0
#define AMPLITUDE 0.2
#define PI 3.14159265358979323846

/* Playback state */
bool	      play_sine = 0;
static double phase	= 0.0;

/* PortAudio callback */
static int pa_callback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer,
		       const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
{
	float* out = (float*)outputBuffer;
	(void)inputBuffer;
	(void)timeInfo;
	(void)statusFlags;
	(void)userData;

	for (unsigned long i = 0; i < framesPerBuffer; i++) {
		float sample = play_sine ? (float)(AMPLITUDE * sin(phase)) : 0.0f;
		*out++	     = sample;	// left
		*out++	     = sample;	// right
		phase += 2.0 * PI * FREQ / SAMPLE_RATE;
		if (phase > 2.0 * PI)
			phase -= 2.0 * PI;
	}
	return paContinue;
}

static Uint32 last_tick = 0;

int main(void)
{
	lv_init();
	lv_display_t* disp = ui_backend_init();
	ui_init(disp);

	// initialize PortAudio
	Pa_Initialize();
	PaStream* stream;
	Pa_OpenDefaultStream(&stream, 0, 2,  // no input, 2 output channels
			     paFloat32, SAMPLE_RATE,
			     256,  // frames per buffer
			     pa_callback, NULL);
	Pa_StartStream(stream);

	last_tick = SDL_GetTicks();

	while (1) {
		// Update LVGL tick
		Uint32 now = SDL_GetTicks();
		lv_tick_inc(now - last_tick);
		last_tick = now;

		// Poll SDL events
		poll_sdl_events();

		// Handle LVGL timers
		lv_timer_handler();

		SDL_Delay(1);  // small sleep
	}

	/* Pa_StopStream(stream); */
	/* Pa_CloseStream(stream); */
	/* Pa_Terminate(); */
}
