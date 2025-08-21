#define LV_CONF_INCLUDE_SIMPLE

#include "ui_driver.h"
#include "lvgl.h"
#include "lv_drivers/display/monitor.h"
#include "lv_drivers/indev/mouse.h"

static void pc_ui_flush(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* color_p)
{
	lv_disp_flush_ready(drv);
}

static void pc_ui_read(lv_indev_drv_t* drv, lv_indev_data_t* data)
{
	/* mouse reading from LVGL SDL driver */
	mouse_read(data);
}

static void pc_ui_init(void)
{
	lv_init();

	/* Init SDL display and mouse */
	monitor_init();
	mouse_init();

	/* Automatically scale window for readability */
	int scale = 1;
	if (LV_HOR_RES <= 320) {
		scale = 3;
	} else if (LV_HOR_RES <= 480) {
		scale = 2;
	}

	lv_sdl_window_set_scale(scale);
	lv_sdl_window_set_title("My Synth");

	/* Register display driver */
	lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.flush_cb = monitor_flush;
	lv_disp_drv_register(&disp_drv);
}

ui_driver_t ui_driver = { .init = pc_ui_init, .flush = pc_ui_flush, .read_input = pc_ui_read };
