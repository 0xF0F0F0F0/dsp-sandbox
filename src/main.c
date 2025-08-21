#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "ui.h"
#include "ui_backend.h"
#include "portaudio.h"

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
static int    play_sine = 0;
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

/* Button click callback */
static void btn_event_cb(lv_event_t* e)
{
	if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
		play_sine = !play_sine;	 // toggle playback
	}
}

int main(void)
{
	/* Initialize platform backend and LVGL */
	lv_display_t* disp = ui_backend_init();
	if (disp == NULL) {
		fprintf(stderr, "ui_backend init failed\n");
		exit(1);
	}
	ui_init(disp);

	/* Create a button in the center */
	lv_obj_t* btn = lv_btn_create(lv_scr_act());
	lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL);

	lv_obj_t* label = lv_label_create(btn);
	lv_label_set_text(label, "Play 200Hz");
	lv_obj_center(label);

	/* Initialize PortAudio */
	/* Pa_Initialize(); */
	/* PaStream* stream; */
	/* Pa_OpenDefaultStream(&stream, 0, 2,  // no input, 2 output channels */
	/* 		     paFloat32, SAMPLE_RATE, */
	/* 		     256,  // frames per buffer */
	/* 		     pa_callback, NULL); */
	/* Pa_StartStream(stream); */

	/* Main loop */
	while (1) {
		lv_timer_handler();  // LVGL tick
		usleep(5 * 1000);
	}

	/* Cleanup (unreachable in this simple loop) */
	/* Pa_StopStream(stream); */
	/* Pa_CloseStream(stream); */
	/* Pa_Terminate(); */

	return 0;
}
