#include <stdlib.h>
#include <stdio.h>
#include "ui.h"
#include "ui_backend.h"
#include "portaudio.h"
#include <SDL2/SDL.h>
#include <arm_math.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define SAMPLE_RATE 44100
#define FREQ 200.0
#define AMPLITUDE 0.2

/* Playback state */
bool		 play_sine = false;
static float32_t phase	   = 0.0;
static float32_t phase_inc = 2.0f * PI * FREQ / SAMPLE_RATE;

static void generate_sine(float* out, uint32_t n)
{
	for (uint32_t i = 0; i < n; i++) {
		float32_t sample = play_sine ? arm_sin_f32(phase) : 0.0f;
		*out++		 = sample;  // left
		*out++		 = sample;  // right
		phase += phase_inc;
		if (phase >= 2.0f * PI) {
			phase -= 2.0f * PI;
		}
	}
}

/* PortAudio callback */
static int pa_callback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer,
		       const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
{
	(void)inputBuffer;
	(void)timeInfo;
	(void)statusFlags;
	(void)userData;

	float* out = (float*)outputBuffer;
	generate_sine(out, framesPerBuffer);

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
