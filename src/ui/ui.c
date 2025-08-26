#include <stdio.h>
#include <string.h>
#include "ui.h"
#include "scales/scales.h"

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

// Sequencer step state (16 steps, each can be on/off)
static bool sequencer_steps[16] = { false };

// Step parameters for each step
static step_params_t step_params[16] = { 0 };
static int	     current_scale   = 0;  // Index of current scale

// Popup objects
static lv_obj_t* scale_popup	    = NULL;
static lv_obj_t* step_popup	    = NULL;
static int	 current_step_index = -1;
static lv_obj_t* scale_btn_label    = NULL;

static void close_scale_popup(lv_event_t* e)
{
	if (scale_popup) {
		lv_obj_del(scale_popup);
		scale_popup = NULL;
	}
}

static void scale_selected(lv_event_t* e)
{
	lv_obj_t* btn	      = lv_event_get_target(e);
	int	  scale_index = (int)(intptr_t)lv_event_get_user_data(e);
	current_scale	      = scale_index;
	
	// Update scale button label
	if (scale_btn_label) {
		lv_label_set_text(scale_btn_label, get_scale_name(current_scale));
	}
	
	close_scale_popup(e);
}

static void create_scale_popup(void)
{
	if (scale_popup)
		return;

	scale_popup = lv_obj_create(lv_scr_act());
	lv_obj_set_size(scale_popup, 200, 250);
	lv_obj_center(scale_popup);
	lv_obj_add_style(scale_popup, &style_tab_container, 0);

	lv_obj_t* title = lv_label_create(scale_popup);
	lv_label_set_text(title, "Select Scale");
	lv_obj_add_style(title, &style_button_label, 0);
	lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

	for (int i = 0; i < get_scale_count(); i++) {
		lv_obj_t* btn = lv_btn_create(scale_popup);
		lv_obj_add_style(btn, &style_button, 0);
		lv_obj_set_size(btn, 180, 30);
		lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 40 + i * 35);

		lv_obj_t* label = lv_label_create(btn);
		lv_label_set_text(label, get_scale_name(i));
		lv_obj_add_style(label, &style_button_label, 0);
		lv_obj_center(label);

		lv_obj_add_event_cb(btn, scale_selected, LV_EVENT_CLICKED, (void*)(intptr_t)i);
	}

}

static void scale_btn_event_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		if (scale_popup) {
			close_scale_popup(e);
		} else {
			create_scale_popup();
		}
	}
}

static void close_step_popup(lv_event_t* e)
{
	if (step_popup) {
		lv_obj_del(step_popup);
		step_popup	   = NULL;
		current_step_index = -1;
	}
}

static void accent_toggle_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		lv_obj_t* btn = lv_event_get_target(e);
		int step_idx = (int)(intptr_t)lv_event_get_user_data(e);
		
		// Toggle accent state
		step_params[step_idx].accent = !step_params[step_idx].accent;
		
		// Update button appearance
		if (step_params[step_idx].accent) {
			lv_obj_set_style_bg_color(btn, lv_color_hex(0x00AA22), LV_PART_MAIN);
		} else {
			lv_obj_set_style_bg_color(btn, lv_color_hex(0x333333), LV_PART_MAIN);
		}
	}
}

static void slide_toggle_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		lv_obj_t* btn = lv_event_get_target(e);
		int step_idx = (int)(intptr_t)lv_event_get_user_data(e);
		
		// Toggle slide state
		step_params[step_idx].slide = !step_params[step_idx].slide;
		
		// Update button appearance
		if (step_params[step_idx].slide) {
			lv_obj_set_style_bg_color(btn, lv_color_hex(0x00AA22), LV_PART_MAIN);
		} else {
			lv_obj_set_style_bg_color(btn, lv_color_hex(0x333333), LV_PART_MAIN);
		}
	}
}

static void note_dropdown_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_VALUE_CHANGED) {
		lv_obj_t* dropdown = lv_event_get_target(e);
		int step_idx = (int)(intptr_t)lv_event_get_user_data(e);
		
		// Update note selection
		step_params[step_idx].note = lv_dropdown_get_selected(dropdown);
	}
}

static void create_step_popup(int step_index)
{
	if (step_popup)
		return;

	current_step_index = step_index;
	step_popup	   = lv_obj_create(lv_scr_act());
	lv_obj_set_size(step_popup, 250, 300);
	lv_obj_center(step_popup);
	lv_obj_add_style(step_popup, &style_tab_container, 0);
	
	// Add 80% transparency to the popup
	lv_obj_set_style_bg_opa(step_popup, LV_OPA_80, LV_PART_MAIN);

	char title_text[32];
	snprintf(title_text, sizeof(title_text), "Step %d", step_index + 1);
	lv_obj_t* title = lv_label_create(step_popup);
	lv_label_set_text(title, title_text);
	lv_obj_add_style(title, &style_button_label, 0);
	lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

	// Note selection dropdown
	lv_obj_t* note_label = lv_label_create(step_popup);
	lv_label_set_text(note_label, "Note:");
	lv_obj_add_style(note_label, &style_button_label, 0);
	lv_obj_align(note_label, LV_ALIGN_TOP_LEFT, 10, 50);

	lv_obj_t* note_dropdown = lv_dropdown_create(step_popup);
	lv_obj_set_size(note_dropdown, 200, 30);
	lv_obj_align(note_dropdown, LV_ALIGN_TOP_MID, 0, 70);
	// Style dropdown main button to match theme
	lv_obj_set_style_bg_color(note_dropdown, lv_color_black(), LV_PART_MAIN);
	lv_obj_set_style_text_color(note_dropdown, lv_color_hex(0xAAAAAA), LV_PART_MAIN);
	lv_obj_set_style_border_color(note_dropdown, lv_color_hex(0x666666), LV_PART_MAIN);
	lv_obj_set_style_border_width(note_dropdown, 1, LV_PART_MAIN);
	lv_obj_set_style_radius(note_dropdown, 3, LV_PART_MAIN);
	

	// Build note options based on current scale
	const int* intervals	= get_scale_intervals(current_scale);
	char	   options[256] = "";
	int note_count = 0;
	for (int i = 0; i < 8 && intervals[i] >= 0; i++) {
		if (i > 0)
			strcat(options, "\n");
		strcat(options, get_note_name(intervals[i] % 12));
		note_count++;
	}
	lv_dropdown_set_options(note_dropdown, options);
	
	// Set selected note (clamp to available notes in current scale)
	int selected_note = step_params[step_index].note;
	if (selected_note >= note_count) {
		selected_note = 0;
		step_params[step_index].note = 0; // Update stored value
	}
	lv_dropdown_set_selected(note_dropdown, selected_note);
	
	// Create a custom style for the dropdown list
	static lv_style_t dropdown_list_style;
	lv_style_init(&dropdown_list_style);
	lv_style_set_bg_color(&dropdown_list_style, lv_color_black());
	lv_style_set_text_color(&dropdown_list_style, lv_color_hex(0xAAAAAA));
	lv_style_set_border_color(&dropdown_list_style, lv_color_hex(0x666666));
	lv_style_set_border_width(&dropdown_list_style, 1);
	
	// Style for selected items
	static lv_style_t dropdown_selected_style;
	lv_style_init(&dropdown_selected_style);
	lv_style_set_bg_color(&dropdown_selected_style, lv_color_hex(0x005522));
	lv_style_set_text_color(&dropdown_selected_style, lv_color_hex(0xCCCCCC));
	
	// Apply styles to dropdown list
	lv_obj_t* dropdown_list = lv_dropdown_get_list(note_dropdown);
	if (dropdown_list) {
		lv_obj_add_style(dropdown_list, &dropdown_list_style, LV_PART_MAIN);
		lv_obj_add_style(dropdown_list, &dropdown_selected_style, LV_PART_SELECTED);
		lv_obj_add_style(dropdown_list, &dropdown_selected_style, LV_PART_SELECTED | LV_STATE_CHECKED);
		lv_obj_add_style(dropdown_list, &dropdown_selected_style, LV_PART_SELECTED | LV_STATE_PRESSED);
	}
	
	// Add event callback for dropdown
	lv_obj_add_event_cb(note_dropdown, note_dropdown_cb, LV_EVENT_VALUE_CHANGED, (void*)(intptr_t)step_index);

	// Accent toggle button and label
	lv_obj_t* accent_btn = lv_btn_create(step_popup);
	lv_obj_set_size(accent_btn, 20, 20);
	lv_obj_align(accent_btn, LV_ALIGN_TOP_LEFT, 20, 120);
	// Style button based on accent state
	if (step_params[step_index].accent) {
		lv_obj_set_style_bg_color(accent_btn, lv_color_hex(0x00AA22), LV_PART_MAIN);
	} else {
		lv_obj_set_style_bg_color(accent_btn, lv_color_hex(0x333333), LV_PART_MAIN);
	}
	lv_obj_set_style_border_color(accent_btn, lv_color_hex(0x666666), LV_PART_MAIN);
	lv_obj_set_style_border_width(accent_btn, 1, LV_PART_MAIN);
	lv_obj_set_style_radius(accent_btn, 2, LV_PART_MAIN);
	
	lv_obj_t* accent_label = lv_label_create(step_popup);
	lv_label_set_text(accent_label, "Accent");
	lv_obj_add_style(accent_label, &style_button_label, 0);
	lv_obj_align(accent_label, LV_ALIGN_TOP_LEFT, 50, 125);
	
	// Add event callback for accent button
	lv_obj_add_event_cb(accent_btn, accent_toggle_cb, LV_EVENT_CLICKED, (void*)(intptr_t)step_index);

	// Slide toggle button and label
	lv_obj_t* slide_btn = lv_btn_create(step_popup);
	lv_obj_set_size(slide_btn, 20, 20);
	lv_obj_align(slide_btn, LV_ALIGN_TOP_LEFT, 20, 160);
	// Style button based on slide state
	if (step_params[step_index].slide) {
		lv_obj_set_style_bg_color(slide_btn, lv_color_hex(0x00AA22), LV_PART_MAIN);
	} else {
		lv_obj_set_style_bg_color(slide_btn, lv_color_hex(0x333333), LV_PART_MAIN);
	}
	lv_obj_set_style_border_color(slide_btn, lv_color_hex(0x666666), LV_PART_MAIN);
	lv_obj_set_style_border_width(slide_btn, 1, LV_PART_MAIN);
	lv_obj_set_style_radius(slide_btn, 2, LV_PART_MAIN);
	
	lv_obj_t* slide_label = lv_label_create(step_popup);
	lv_label_set_text(slide_label, "Slide");
	lv_obj_add_style(slide_label, &style_button_label, 0);
	lv_obj_align(slide_label, LV_ALIGN_TOP_LEFT, 50, 165);
	
	// Add event callback for slide button
	lv_obj_add_event_cb(slide_btn, slide_toggle_cb, LV_EVENT_CLICKED, (void*)(intptr_t)step_index);

	// Add click outside to close functionality
	lv_obj_add_flag(step_popup, LV_OBJ_FLAG_CLICK_FOCUSABLE);
	lv_obj_add_event_cb(lv_scr_act(), close_step_popup, LV_EVENT_CLICKED, NULL);
}

// Flag to prevent click after long press
static bool long_press_occurred = false;

static void step_button_event_cb(lv_event_t* e)
{
	lv_event_code_t code	   = lv_event_get_code(e);
	lv_obj_t*	btn	   = (lv_obj_t*)lv_event_get_target(e);
	int		step_index = (int)(intptr_t)lv_event_get_user_data(e);

	if (code == LV_EVENT_LONG_PRESSED) {
		// Set flag and open step parameter popup
		long_press_occurred = true;
		create_step_popup(step_index);
	} else if (code == LV_EVENT_CLICKED) {
		// Only toggle if it wasn't a long press
		if (long_press_occurred) {
			long_press_occurred = false; // Reset flag
			return; // Don't toggle on long press
		}
		
		// Toggle step state
		sequencer_steps[step_index] = !sequencer_steps[step_index];

		// Update button appearance
		if (sequencer_steps[step_index]) {
			lv_obj_set_style_bg_color(btn, lv_color_hex(0x00AA22), LV_PART_MAIN);
		} else {
			lv_obj_set_style_bg_color(btn, lv_color_hex(0x333333), LV_PART_MAIN);
		}
	}
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

	// Grid container centered on screen
	lv_obj_t* grid_container = lv_obj_create(sequencer_screen);
	lv_obj_add_style(grid_container, &style_tab_container, 0);
	lv_obj_set_size(grid_container, 240, 240);
	lv_obj_center(grid_container);
	lv_obj_set_layout(grid_container, LV_LAYOUT_GRID);
	lv_obj_set_style_pad_all(grid_container, 8, 0);
	lv_obj_set_style_pad_gap(grid_container, 4, 0);

	// Define grid: 4 columns, 4 rows
	static int32_t col_dsc[] = { LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
				     LV_GRID_TEMPLATE_LAST };
	static int32_t row_dsc[] = { LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
				     LV_GRID_TEMPLATE_LAST };
	lv_obj_set_grid_dsc_array(grid_container, col_dsc, row_dsc);

	// Create 4x4 grid of buttons (16 total)
	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++) {
			int step_index = row * 4 + col;

			lv_obj_t* btn = lv_btn_create(grid_container);
			lv_obj_set_size(btn, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
			lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_STRETCH, col, 1, LV_GRID_ALIGN_STRETCH, row, 1);

			// Style the button to be square
			lv_obj_set_style_bg_color(btn, lv_color_hex(0x333333), LV_PART_MAIN);
			lv_obj_set_style_border_color(btn, lv_color_hex(0x666666), LV_PART_MAIN);
			lv_obj_set_style_border_width(btn, 1, LV_PART_MAIN);
			lv_obj_set_style_radius(btn, 2, LV_PART_MAIN);

			// Add event callbacks for click and long press with step index as user data
			lv_obj_add_event_cb(btn, step_button_event_cb, LV_EVENT_CLICKED, (void*)(intptr_t)step_index);
			lv_obj_add_event_cb(btn, step_button_event_cb, LV_EVENT_LONG_PRESSED,
					    (void*)(intptr_t)step_index);
		}
	}

	// Scale button in top right corner
	lv_obj_t* scale_btn = lv_btn_create(sequencer_screen);
	lv_obj_add_style(scale_btn, &style_scale_button, 0);
	lv_obj_set_size(scale_btn, 80, 30);
	lv_obj_align(scale_btn, LV_ALIGN_TOP_RIGHT, -10, 10);
	scale_btn_label = lv_label_create(scale_btn);
	lv_label_set_text(scale_btn_label, get_scale_name(current_scale));
	lv_obj_add_style(scale_btn_label, &style_step_label, 0);
	lv_obj_center(scale_btn_label);
	lv_obj_add_event_cb(scale_btn, scale_btn_event_cb, LV_EVENT_CLICKED, NULL);

	// Menu positioned at bottom
	lv_obj_t* menu = lv_obj_create(sequencer_screen);
	lv_obj_add_style(menu, &style_tab_container, 0);
	lv_obj_set_size(menu, lv_pct(100), lv_pct(10));
	lv_obj_align(menu, LV_ALIGN_BOTTOM_MID, 0, 0);
	lv_obj_set_flex_flow(menu, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(menu, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_clear_flag(menu, LV_OBJ_FLAG_SCROLLABLE);
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
