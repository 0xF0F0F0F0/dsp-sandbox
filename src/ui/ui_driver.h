#ifndef UI_DRIVER_H
#define UI_DRIVER_H

#include "lvgl.h"

/* UI driver abstraction for LVGL 9.3 */
typedef struct {
    void (*init)(void);
    void (*flush)(lv_display_t *disp, const lv_area_t *area, lv_color_t *color_p);
    void (*read_input)(lv_indev_t *indev, lv_indev_data_t *data);
} ui_driver_t;

extern ui_driver_t ui_driver;

#endif	// UI_DRIVER_H
