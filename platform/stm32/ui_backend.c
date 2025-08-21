#define LV_CONF_INCLUDE_SIMPLE

#include "ui_driver.h"
#include "lvgl.h"
#include "stm32_lcd.h"
#include "stm32_touch.h"

static void stm32_ui_flush(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* color_p)
{
	lcd_draw_area(area->x1, area->y1, area->x2, area->y2, color_p);
	lv_disp_flush_ready(drv);
}

static void stm32_ui_read(lv_indev_drv_t* drv, lv_indev_data_t* data)
{
	touch_read(data);
}

static void stm32_ui_init(void)
{
	lv_init();

	/* Initialize display driver (e.g., DMA2D or LCD) */
	stm32_disp_init();

	/* Initialize input devices (touchscreen, buttons) */
	stm32_indev_init();

	/* Register display driver */
	lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.flush_cb = stm32_flush;
	lv_disp_drv_register(&disp_drv);
}

ui_driver_t ui_driver = { .init = stm32_ui_init, .flush = stm32_ui_flush, .read_input = stm32_ui_read };
