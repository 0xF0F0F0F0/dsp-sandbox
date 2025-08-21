#include "ui_backend.h"
#include "lvgl.h"
#include "stm32f4xx_hal.h"  // replace with your MCU header

static lv_color_t    buf1[LV_HOR_RES * 40];
static lv_display_t* disp;

/* Flush callback for LVGL 9.x */
static void flush_cb(lv_display_t* d, const lv_area_t* area, uint8_t* color_p)
{
	lv_color_t* buf = (lv_color_t*)color_p;

	int32_t x1 = area->x1;
	int32_t y1 = area->y1;
	int32_t w  = lv_area_get_width(area);
	int32_t h  = lv_area_get_height(area);

	// TODO: Replace with your STM32 LCD drawing function
	// Example: LCD_DrawBitmap(x1, y1, w, h, buf);

	lv_display_flush_ready(d);
}

/* Touch input callback */
static void touch_read_cb(lv_indev_t* indev, lv_indev_data_t* data)
{
	data->state   = LV_INDEV_STATE_RELEASED;
	data->point.x = 0;
	data->point.y = 0;

	// TODO: Replace with your touchscreen driver
	// if(Touch_Pressed()) {
	//     data->state = LV_INDEV_STATE_PRESSED;
	//     Touch_GetXY(&data->point.x, &data->point.y);
	// }
}

/* Public init function */
void ui_backend_init(void)
{
	// Initialize LCD, DMA2D, Touch, etc.
	// LCD_Init();
	// Touch_Init();

	disp = lv_display_create(LV_HOR_RES, LV_VER_RES);
	lv_display_set_flush_cb(disp, flush_cb);
	lv_display_set_buffers(disp, buf1, NULL, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);

	lv_indev_t* touch = lv_indev_create();
	lv_indev_set_type(touch, LV_INDEV_TYPE_POINTER);
	lv_indev_set_read_cb(touch, touch_read_cb);
}
