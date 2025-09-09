#ifndef PLATFORM_UI_DRIVER_H
#define PLATFORM_UI_DRIVER_H

#include "lvgl.h"
#include <stdbool.h>

// Platform-agnostic UI driver interface
lv_display_t* ui_driver_init(void);
void	  ui_driver_poll_events(void);
void	  ui_driver_cleanup(void);

#endif  // PLATFORM_UI_DRIVER_H
