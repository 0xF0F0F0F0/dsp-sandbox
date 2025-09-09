#include <stdio.h>
#include "ui.h"
#include "ui_driver.h"
#include "audio_driver.h"
#include "platform.h"

static unsigned int last_tick = 0;

int main(void)
{
	// Initialize platform
	if (!platform_init()) {
		fprintf(stderr, "Failed to initialize platform\n");
		return 1;
	}

	// Initialize LVGL
	lv_init();
	lv_display_t* disp = ui_driver_init();
	ui_init(disp);

	// DSP is now initialized per-port in audio callbacks

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

	last_tick = platform_get_ticks();

	while (1) {
		// Update LVGL tick
		unsigned int now = platform_get_ticks();
		lv_tick_inc(now - last_tick);
		last_tick = now;

		// Poll UI events
		ui_driver_poll_events();

		// Handle LVGL timers
		lv_timer_handler();

		platform_delay_ms(1);
	}

	// Cleanup (unreachable in current implementation)
	audio_driver_stop();
	audio_driver_cleanup();
	platform_cleanup();

	return 0;
}
