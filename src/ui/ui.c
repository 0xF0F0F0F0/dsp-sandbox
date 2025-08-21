#include <stdio.h>
#include "ui.h"

extern bool play_sine;

static void btn_event_cb(lv_event_t* e)
{
	if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
		play_sine = !play_sine;	 // toggle playback
		printf("Button clicked, play_sine = %d\n", play_sine);
	}
}

void ui_init(lv_display_t* disp)
{
	LV_UNUSED(disp);  // if not needed

	lv_obj_t* scr = lv_scr_act();  // active screen

	// Create a button
	lv_obj_t* btn = lv_btn_create(scr);
	lv_obj_align(btn, LV_ALIGN_CENTER, 0, 40);
	lv_obj_set_size(btn, 200, 100);
	lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL);

	lv_obj_t* btn_label = lv_label_create(btn);
	lv_label_set_text(btn_label, "Play 200Hz");
	lv_obj_center(btn_label);
}
