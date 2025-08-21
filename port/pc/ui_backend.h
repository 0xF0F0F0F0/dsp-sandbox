#ifndef UI_BACKEND_H
#define UI_BACKEND_H

#include "lvgl.h"

/* Initialize the platform-specific backend (LCD + input) */
lv_display_t* ui_backend_init(void);

#endif	// UI_BACKEND_H
