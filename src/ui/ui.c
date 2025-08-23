#include <stdio.h>
#include "ui.h"

static lv_obj_t* params_screen;
static lv_obj_t* seq_screen;
static lv_obj_t* tabview;

extern bool play_sine;

static void btn_event_cb(lv_event_t* e)
{
	if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
		play_sine = !play_sine;	 // toggle playback
		printf("Button clicked, play_sine = %d\n", play_sine);
	}
}

static lv_obj_t* create_tab_flex(lv_obj_t* tab)
{
	lv_obj_t* cont = lv_obj_create(tab);
	lv_obj_set_style_border_width(cont, 0, 0);
	lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
	lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

	return cont;
}

static lv_obj_t* create_slider(lv_obj_t* cont, const char* name)
{
	lv_obj_set_style_bg_color(cont, lv_color_black(), 0);
	lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);

	lv_obj_t* slider = lv_slider_create(cont);
	lv_obj_set_style_bg_color(slider, lv_color_hex(0x777777), LV_PART_MAIN);
	lv_obj_set_style_bg_color(slider, lv_color_hex(0x005522), LV_PART_INDICATOR);
	lv_obj_set_style_bg_color(slider, lv_color_hex(0x00AA22), LV_PART_KNOB);
	lv_obj_set_size(slider, 10, lv_pct(100));
	lv_slider_set_range(slider, 0, 100);
	lv_slider_set_value(slider, 50, LV_ANIM_ON);

	lv_obj_t* label = lv_label_create(cont);
	lv_obj_set_style_text_color(label, lv_color_hex(0x777777), 0);
	lv_label_set_text(label, name);
	lv_obj_center(label);

	return slider;
}

static void create_tab_1(lv_obj_t* tab)
{
	lv_obj_t* cont = create_tab_flex(tab);
	create_slider(cont, "CUT");
	create_slider(cont, "RES");
	create_slider(cont, "ENV");
	create_slider(cont, "ACT");
}

static void create_tab_2(lv_obj_t* tab)
{
	lv_obj_t* cont = create_tab_flex(tab);
	create_slider(cont, "ATK");
	create_slider(cont, "DEC");
	create_slider(cont, "SUS");
	create_slider(cont, "REL");
}

static void create_tab_3(lv_obj_t* tab)
{
	lv_obj_t* cont = create_tab_flex(tab);
	create_slider(cont, "DST");
	create_slider(cont, "DEL");
	create_slider(cont, "REV");
	create_slider(cont, "VOL");
}

static lv_obj_t* create_menu_flex(lv_obj_t* tab)
{
	lv_obj_t* cont = lv_obj_create(tab);
	lv_obj_set_style_border_width(cont, 0, 0);
	lv_obj_set_style_bg_color(cont, lv_color_black(), 0);
	lv_obj_set_size(cont, lv_pct(100), lv_pct(10));
	lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
	return cont;
}

static void create_menu_buttons(lv_obj_t* menu)
{
	int btn_height = 20;
	int btn_width  = 60;

	lv_obj_t* seq_btn = lv_btn_create(menu);
	lv_obj_set_size(seq_btn, btn_width, btn_height);
	lv_obj_t* seq_label = lv_label_create(seq_btn);
	lv_label_set_text(seq_label, "SEQ");
	lv_obj_center(seq_label);

	lv_obj_t* osc_btn = lv_btn_create(menu);
	lv_obj_set_size(osc_btn, btn_width, btn_height);
	lv_obj_t* osc_label = lv_label_create(osc_btn);
	lv_label_set_text(osc_label, "OSC");
	lv_obj_center(osc_label);

	lv_obj_t* key_btn = lv_btn_create(menu);
	lv_obj_set_size(key_btn, btn_width, btn_height);
	lv_obj_t* key_label = lv_label_create(key_btn);
	lv_label_set_text(key_label, "KEY");
	lv_obj_center(key_label);

	lv_obj_t* set_btn = lv_btn_create(menu);
	lv_obj_set_size(set_btn, btn_width, btn_height);
	lv_obj_t* set_label = lv_label_create(set_btn);
	lv_label_set_text(set_label, "SET");
	lv_obj_center(set_label);

	uint32_t cnt = lv_obj_get_child_cnt(menu);
	for (uint32_t i = 0; i < cnt; i++) {
		lv_obj_t* btn = lv_obj_get_child(menu, i);
		lv_obj_set_style_bg_color(btn, lv_color_black(), 0);
		lv_obj_set_style_text_color(btn, lv_color_hex(0xAAAAAA), 0);
	}
}

static lv_obj_t* create_params_screen(void)
{
	params_screen = lv_obj_create(NULL);

	// tab view
	tabview = lv_tabview_create(params_screen);
	lv_tabview_set_tab_bar_size(tabview, 5);
	lv_tabview_set_tab_bar_position(tabview, LV_DIR_TOP);
	lv_obj_set_style_bg_color(tabview, lv_color_black(), 0);

	// tab bar
	lv_obj_t* bar = lv_tabview_get_tab_bar(tabview);
	lv_obj_set_style_bg_color(bar, lv_color_black(), LV_PART_MAIN);

	// individual tabs
	lv_obj_t* tab1 = lv_tabview_add_tab(tabview, "");
	create_tab_1(tab1);

	lv_obj_t* tab2 = lv_tabview_add_tab(tabview, "");
	create_tab_2(tab2);

	lv_obj_t* tab3 = lv_tabview_add_tab(tabview, "");
	create_tab_3(tab3);

	// color the tab buttons
	uint32_t cnt = lv_obj_get_child_cnt(bar);
	for (uint32_t i = 0; i < cnt; i++) {
		lv_obj_t* btn = lv_obj_get_child(bar, i);

		// Set inactive style
		lv_obj_set_style_bg_color(btn, lv_color_black(), 0);
		lv_obj_set_style_text_color(btn, lv_color_hex(0xAAAAAA), 0);

		// Set checked style (active tab)
		lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_CHECKED);
		lv_obj_set_style_bg_color(btn, lv_color_hex(0x005522), LV_PART_MAIN | LV_STATE_CHECKED);
		lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN | LV_STATE_CHECKED);
		lv_obj_set_style_text_color(btn, lv_color_hex(0xCCCCCC), LV_STATE_CHECKED);
	}

	lv_obj_t* menu = create_menu_flex(tabview);
	create_menu_buttons(menu);

	return params_screen;
}

static lv_obj_t* create_seq_screen(void)
{
	seq_screen	= lv_obj_create(NULL);
	lv_obj_t* label = lv_label_create(seq_screen);
	lv_label_set_text(label, "Blank screen");
	lv_obj_center(label);

	return seq_screen;
}

void switch_to_seq_screen(void)
{
	lv_scr_load_anim(seq_screen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
}

void switch_to_params_screen(void)
{
	lv_scr_load_anim(params_screen, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, false);
}

void ui_init(lv_display_t* disp)
{
	params_screen = create_params_screen();
	seq_screen    = create_seq_screen();

	lv_scr_load(params_screen);
}
