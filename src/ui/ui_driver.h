#ifndef UI_DRIVER_H
#define UI_DRIVER_H

#include "lvgl.h"

typedef struct {
	void (*init)(void);
	void (*flush)(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* color_p);
	void (*read_input)(lv_indev_drv_t* drv, lv_indev_data_t* data);
} ui_driver_t;

extern ui_driver_t ui_driver;

#endif
