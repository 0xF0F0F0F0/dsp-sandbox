#include "ui.h"
#include "lvgl.h"

void ui_init(lv_display_t* disp)
{
	LV_UNUSED(disp);  // optional: if not needed directly

	lv_obj_t* label = lv_label_create(lv_scr_act());
	lv_label_set_text(label, "Hello LVGL!");
	lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

	lv_obj_t* btn = lv_btn_create(lv_scr_act());
	lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -20);
	lv_obj_t* btn_label = lv_label_create(btn);
	lv_label_set_text(btn_label, "Click Me");
	lv_obj_center(btn_label);
}
