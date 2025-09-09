#ifndef UI_BACKEND_H
#define UI_BACKEND_H

#include "lvgl.h"

/* PC-specific UI backend implementation */
lv_display_t* ui_backend_init(void);
void	  poll_sdl_events(void);
void	  ui_backend_cleanup(void);

#endif  // UI_BACKEND_H
