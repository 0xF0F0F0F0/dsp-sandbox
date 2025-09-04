#include <stdio.h>
#include "ui.h"
#include "ui_backend.h"
#include "audio_driver.h"
#include "dsp.h"
#include <SDL2/SDL.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

static Uint32 last_tick = 0;

int main(void)
{
	// Initialize LVGL
	lv_init();
	lv_display_t* disp = ui_backend_init();
	ui_init(disp);

	// Initialize DSP engine
	dsp_init();

	// Initialize audio driver
	if (!audio_driver_init()) {
		fprintf(stderr, "Failed to initialize audio driver\n");
		return 1;
	}

	// Start audio processing
	if (!audio_driver_start()) {
		fprintf(stderr, "Failed to start audio driver\n");
		audio_driver_cleanup();
		return 1;
	}

	printf("Audio system started successfully\n");

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

	// Cleanup (unreachable in current implementation)
	audio_driver_stop();
	audio_driver_cleanup();
	dsp_cleanup();

	return 0;
}
