#include "ui_driver.h"
#include "ui_backend.h"

lv_display_t* ui_driver_init(void)
{
	return ui_backend_init();
}

void ui_driver_poll_events(void)
{
	poll_sdl_events();
}

void ui_driver_cleanup(void)
{
	ui_backend_cleanup();
}