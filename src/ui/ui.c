#include <stdio.h>
#include "ui.h"

// Screens
static lv_obj_t* params_screen;
static lv_obj_t* sequencer_screen;
static lv_obj_t* tabview;

extern bool play_sine;

// --------------------------
// Styles
// --------------------------
static lv_style_t style_screen;
static lv_style_t style_tab_container;
static lv_style_t style_slider;
static lv_style_t style_slider_indicator;
static lv_style_t style_slider_knob;
static lv_style_t style_button;
static lv_style_t style_button_label;
static lv_style_t style_step_container;
static lv_style_t style_step_button;
static lv_style_t style_step_label;
static lv_style_t style_scale_button;

static void init_styles(void)
{
	// Screen background
	lv_style_init(&style_screen);
	lv_style_set_bg_color(&style_screen, lv_color_black());
	lv_style_set_pad_all(&style_screen, 0);

	// Tab container
	lv_style_init(&style_tab_container);
	lv_style_set_bg_color(&style_tab_container, lv_color_black());
	lv_style_set_border_width(&style_tab_container, 0);
	lv_style_set_pad_all(&style_tab_container, 0);

	// Slider main background
	lv_style_init(&style_slider);
	lv_style_set_bg_color(&style_slider, lv_color_hex(0x777777));
	lv_style_set_radius(&style_slider, 3);

	// Slider indicator
	lv_style_init(&style_slider_indicator);
	lv_style_set_bg_color(&style_slider_indicator, lv_color_hex(0x005522));
	lv_style_set_radius(&style_slider_indicator, 3);

	// Slider knob
	lv_style_init(&style_slider_knob);
	lv_style_set_bg_color(&style_slider_knob, lv_color_hex(0x00AA22));
	lv_style_set_radius(&style_slider_knob, LV_RADIUS_CIRCLE);

	// Generic button
	lv_style_init(&style_button);
	lv_style_set_bg_opa(&style_button, LV_OPA_COVER);
	lv_style_set_bg_color(&style_button, lv_color_black());
	lv_style_set_border_width(&style_button, 0);
	lv_style_set_radius(&style_button, 1);

	// Button label
	lv_style_init(&style_button_label);
	lv_style_set_text_color(&style_button_label, lv_color_hex(0xAAAAAA));

	// Step container
	lv_style_init(&style_step_container);
	lv_style_set_pad_all(&style_step_container, 5);

	// Step button (gate, accent, slide)
	lv_style_init(&style_step_button);
	lv_style_set_bg_color(&style_step_button, lv_color_hex(0x00AA22));
	lv_style_set_radius(&style_step_button, 3);

	// Step label
	lv_style_init(&style_step_label);
	lv_style_set_text_color(&style_step_label, lv_color_white());

	// Scale button
	lv_style_init(&style_scale_button);
	lv_style_set_bg_color(&style_scale_button, lv_color_black());
	lv_style_set_border_width(&style_scale_button, 1);
	lv_style_set_border_color(&style_scale_button, lv_color_hex(0x00AA22));
	lv_style_set_radius(&style_scale_button, 3);
	lv_style_set_text_color(&style_scale_button, lv_color_white());
}

static void par_btn_event_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		lv_scr_load(params_screen);
	}
}

static void seq_btn_event_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		lv_scr_load(sequencer_screen);
	}
}

static void osc_btn_event_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		lv_scr_load(sequencer_screen);
	}
}

static void set_btn_event_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		lv_scr_load(sequencer_screen);
	}
}

static void (*menu_callbacks[])(lv_event_t*) = { par_btn_event_cb, seq_btn_event_cb, seq_btn_event_cb,
						 seq_btn_event_cb };

// --------------------------
// Helper functions
// --------------------------
static lv_obj_t* create_slider(lv_obj_t* cont, const char* name)
{
	lv_obj_t* slider = lv_slider_create(cont);
	lv_obj_add_style(slider, &style_slider, LV_PART_MAIN);
	lv_obj_add_style(slider, &style_slider_indicator, LV_PART_INDICATOR);
	lv_obj_add_style(slider, &style_slider_knob, LV_PART_KNOB);
	lv_obj_set_size(slider, 10, lv_pct(100));
	lv_slider_set_range(slider, 0, 100);
	lv_slider_set_value(slider, 50, LV_ANIM_ON);

	lv_obj_t* label = lv_label_create(cont);
	lv_label_set_text(label, name);
	lv_obj_add_style(label, &style_button_label, 0);
	lv_obj_center(label);

	return slider;
}

static lv_obj_t* create_tab_flex(lv_obj_t* tab)
{
	lv_obj_t* cont = lv_obj_create(tab);
	lv_obj_add_style(cont, &style_tab_container, 0);
	lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
	lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	return cont;
}

// --------------------------
// Tab creation
// --------------------------
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

// --------------------------
// Menu buttons
// --------------------------
static lv_obj_t* create_menu_flex(lv_obj_t* tab)
{
	lv_obj_t* cont = lv_obj_create(tab);
	lv_obj_add_style(cont, &style_tab_container, 0);
	lv_obj_set_size(cont, lv_pct(100), lv_pct(10));
	lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
	return cont;
}

static void create_menu_buttons(lv_obj_t* menu)
{
	const char* names[] = { "PAR", "SEQ", "OSC", "SET" };
	for (int i = 0; i < 4; i++) {
		lv_obj_t* btn = lv_btn_create(menu);
		lv_obj_add_style(btn, &style_button, 0);
		lv_obj_set_size(btn, 60, 20);

		lv_obj_t* label = lv_label_create(btn);
		lv_label_set_text(label, names[i]);
		lv_obj_add_style(label, &style_button_label, 0);
		lv_obj_center(label);

		lv_obj_add_event_cb(btn, menu_callbacks[i], LV_EVENT_CLICKED, NULL);
	}
}

// --------------------------
// Params screen
// --------------------------
static lv_obj_t* create_params_screen(void)
{
	params_screen = lv_obj_create(NULL);
	lv_obj_add_style(params_screen, &style_screen, 0);

	// Tabview
	tabview = lv_tabview_create(params_screen);
	lv_tabview_set_tab_bar_size(tabview, 5);
	lv_tabview_set_tab_bar_position(tabview, LV_DIR_TOP);
	lv_obj_add_style(tabview, &style_screen, 0);

	// Individual tabs
	lv_obj_t* tab1 = lv_tabview_add_tab(tabview, "");
	create_tab_1(tab1);
	lv_obj_t* tab2 = lv_tabview_add_tab(tabview, "");
	create_tab_2(tab2);
	lv_obj_t* tab3 = lv_tabview_add_tab(tabview, "");
	create_tab_3(tab3);

	// Tab button styles
	lv_obj_t* bar = lv_tabview_get_tab_bar(tabview);
	uint32_t  cnt = lv_obj_get_child_cnt(bar);
	for (uint32_t i = 0; i < cnt; i++) {
		lv_obj_t* btn = lv_obj_get_child(bar, i);
		lv_obj_add_style(btn, &style_button, 0);
		// checked state
		lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_CHECKED);
		lv_obj_set_style_bg_color(btn, lv_color_hex(0x005522), LV_PART_MAIN | LV_STATE_CHECKED);
		lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN | LV_STATE_CHECKED);
		lv_obj_set_style_text_color(btn, lv_color_hex(0xCCCCCC), LV_STATE_CHECKED);
	}

	// Menu
	lv_obj_t* menu = create_menu_flex(tabview);
	create_menu_buttons(menu);

	return params_screen;
}

// --------------------------
// Sequencer screen
// --------------------------
static lv_obj_t* create_seq_screen(void)
{
	sequencer_screen = lv_obj_create(NULL);
	lv_obj_add_style(sequencer_screen, &style_screen, 0);

	lv_obj_t* steps_container = lv_obj_create(sequencer_screen);
	lv_obj_add_style(steps_container, &style_tab_container, 0);
	lv_obj_set_size(steps_container, lv_pct(100), lv_pct(100));
	lv_obj_center(steps_container);
	lv_obj_set_flex_flow(steps_container, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(steps_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_pad_column(steps_container, 5, 0);

	for (int i = 0; i < 8; i++) {
		lv_obj_t* step = lv_obj_create(steps_container);
		lv_obj_add_style(step, &style_step_container, 0);
		lv_obj_set_size(step, 45, 110);
		lv_obj_set_flex_flow(step, LV_FLEX_FLOW_COLUMN);
		lv_obj_set_flex_align(step, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

		lv_obj_t* gate_btn = lv_btn_create(step);
		lv_obj_add_style(gate_btn, &style_step_button, 0);
		lv_obj_set_size(gate_btn, 40, 20);
		lv_obj_t* gate_label = lv_label_create(gate_btn);
		lv_label_set_text(gate_label, "Gate");
		lv_obj_add_style(gate_label, &style_step_label, 0);
		lv_obj_center(gate_label);

		lv_obj_t* note_roller = lv_roller_create(step);
		lv_roller_set_options(note_roller, "C\nC#\nD\nD#\nE\nF\nF#\nG\nG#\nA\nA#\nB", LV_ROLLER_MODE_INFINITE);
		lv_obj_set_size(note_roller, 40, 50);

		lv_obj_t* accent_btn = lv_btn_create(step);
		lv_obj_add_style(accent_btn, &style_step_button, 0);
		lv_obj_set_size(accent_btn, 20, 20);
		lv_obj_t* accent_label = lv_label_create(accent_btn);
		lv_label_set_text(accent_label, "A");
		lv_obj_add_style(accent_label, &style_step_label, 0);
		lv_obj_center(accent_label);

		lv_obj_t* slide_btn = lv_btn_create(step);
		lv_obj_add_style(slide_btn, &style_step_button, 0);
		lv_obj_set_size(slide_btn, 20, 20);
		lv_obj_t* slide_label = lv_label_create(slide_btn);
		lv_label_set_text(slide_label, "S");
		lv_obj_add_style(slide_label, &style_step_label, 0);
		lv_obj_center(slide_label);
	}

	lv_obj_t* scale_btn = lv_btn_create(sequencer_screen);
	lv_obj_add_style(scale_btn, &style_scale_button, 0);
	lv_obj_set_size(scale_btn, 120, 40);
	lv_obj_align(scale_btn, LV_ALIGN_TOP_RIGHT, -10, 10);
	lv_obj_t* scale_label = lv_label_create(scale_btn);
	lv_label_set_text(scale_label, "Select Scale");
	lv_obj_add_style(scale_label, &style_step_label, 0);
	lv_obj_center(scale_label);

	// Menu
	lv_obj_t* menu = create_menu_flex(sequencer_screen);
	create_menu_buttons(menu);

	return sequencer_screen;
}

// --------------------------
// UI init
// --------------------------
void ui_init(lv_display_t* disp)
{
	init_styles();

	params_screen	 = create_params_screen();
	sequencer_screen = create_seq_screen();

	lv_scr_load(params_screen);
}
