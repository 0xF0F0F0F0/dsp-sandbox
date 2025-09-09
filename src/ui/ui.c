#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ui.h"
#include "scales.h"
#include "notes.h"
#include "synth_state.h"

// Screens
static lv_obj_t* params_screen;
static lv_obj_t* sequencer_screen;
static lv_obj_t* settings_screen;
static lv_obj_t* tabview;

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
static lv_style_t style_button_label_black;
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
	lv_style_set_bg_color(&style_slider, lv_color_hex(0x333333));
	lv_style_set_radius(&style_slider, 3);

	// Slider indicator (filled portion)
	lv_style_init(&style_slider_indicator);
	lv_style_set_bg_color(&style_slider_indicator, lv_color_hex(0x00AA22));
	lv_style_set_radius(&style_slider_indicator, 3);

	// Slider knob (make it invisible/flat)
	lv_style_init(&style_slider_knob);
	lv_style_set_bg_opa(&style_slider_knob, LV_OPA_TRANSP);
	lv_style_set_border_width(&style_slider_knob, 0);
	lv_style_set_width(&style_slider_knob, 0);
	lv_style_set_height(&style_slider_knob, 0);

	// Generic button
	lv_style_init(&style_button);
	lv_style_set_bg_opa(&style_button, LV_OPA_COVER);
	lv_style_set_bg_color(&style_button, lv_color_black());
	lv_style_set_border_width(&style_button, 0);
	lv_style_set_radius(&style_button, 1);

	// Button label
	lv_style_init(&style_button_label);
	lv_style_set_text_color(&style_button_label, lv_color_hex(0xAAAAAA));

	// Button label with black text (for pattern and control buttons)
	lv_style_init(&style_button_label_black);
	lv_style_set_text_color(&style_button_label_black, lv_color_black());

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
		lv_scr_load(settings_screen);
	}
}

static void (*menu_callbacks[])(lv_event_t*) = {par_btn_event_cb, seq_btn_event_cb, seq_btn_event_cb, set_btn_event_cb};

static void slider_event_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_VALUE_CHANGED) {
		lv_obj_t*     slider = lv_event_get_target(e);
		synth_param_t param  = (synth_param_t)(intptr_t)lv_event_get_user_data(e);

		// Get slider value (0-100) and store in global state
		int32_t value = lv_slider_get_value(slider);
		synth_state_set_parameter(param, (uint8_t)value);

		// Debug output
		const char* param_names[] = {"CUTOFF",  "RESONANCE", "ENVELOPE",   "ACCENT", "ATTACK", "DECAY",
					     "SUSTAIN", "RELEASE",   "DISTORTION", "DELAY",  "REVERB", "VOLUME"};
		printf("Parameter changed: %s = %d\n", param_names[param], (int)value);
	}
}

// --------------------------
// Helper functions
// --------------------------
static lv_obj_t* create_slider(lv_obj_t* cont, const char* name, synth_param_t param)
{
	// Create container for slider and label
	lv_obj_t* slider_cont = lv_obj_create(cont);
	lv_obj_add_style(slider_cont, &style_tab_container, 0);
	lv_obj_set_size(slider_cont, 80, lv_pct(100));
	lv_obj_set_flex_flow(slider_cont, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(slider_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

	// Create label at the bottom, outside the slider
	lv_obj_t* label = lv_label_create(slider_cont);
	lv_label_set_text(label, name);
	lv_obj_add_style(label, &style_button_label, 0);

	// Create vertical slider
	lv_obj_t* slider = lv_slider_create(slider_cont);
	lv_obj_add_style(slider, &style_slider, LV_PART_MAIN);
	lv_obj_add_style(slider, &style_slider_indicator, LV_PART_INDICATOR);
	lv_obj_add_style(slider, &style_slider_knob, LV_PART_KNOB);
	lv_obj_set_size(slider, 30, lv_pct(80));
	lv_slider_set_range(slider, 0, 100);

	// Set initial value from global state
	uint8_t initial_value = synth_state_get_parameter(param);
	lv_slider_set_value(slider, initial_value, LV_ANIM_OFF);

	// Add event callback
	lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, (void*)(intptr_t)param);

	return slider;
}

static lv_obj_t* create_tab_flex(lv_obj_t* tab)
{
	lv_obj_t* cont = lv_obj_create(tab);
	lv_obj_add_style(cont, &style_tab_container, 0);
	lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
	lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_pad_all(cont, 10, 0);
	return cont;
}

// --------------------------
// Tab creation
// --------------------------
static void create_tab_1(lv_obj_t* tab)
{
	lv_obj_t* cont = create_tab_flex(tab);
	create_slider(cont, "CUT", PARAM_CUTOFF);
	create_slider(cont, "RES", PARAM_RESONANCE);
	create_slider(cont, "ENV", PARAM_ENVELOPE);
	create_slider(cont, "ACT", PARAM_ACCENT);
}

static void create_tab_2(lv_obj_t* tab)
{
	lv_obj_t* cont = create_tab_flex(tab);
	create_slider(cont, "ATK", PARAM_ATTACK);
	create_slider(cont, "DEC", PARAM_DECAY);
	create_slider(cont, "SUS", PARAM_SUSTAIN);
	create_slider(cont, "REL", PARAM_RELEASE);
}

static void create_tab_3(lv_obj_t* tab)
{
	lv_obj_t* cont = create_tab_flex(tab);
	create_slider(cont, "DST", PARAM_DISTORTION);
	create_slider(cont, "DEL", PARAM_DELAY);
	create_slider(cont, "REV", PARAM_REVERB);
	create_slider(cont, "VOL", PARAM_VOLUME);
}

// UI state - all actual data now stored in g_synth_state

// Parameter mapping for sliders
static const synth_param_t tab1_params[] = {PARAM_CUTOFF, PARAM_RESONANCE, PARAM_ENVELOPE, PARAM_ACCENT};
static const synth_param_t tab2_params[] = {PARAM_ATTACK, PARAM_DECAY, PARAM_SUSTAIN, PARAM_RELEASE};
static const synth_param_t tab3_params[] = {PARAM_DISTORTION, PARAM_DELAY, PARAM_REVERB, PARAM_VOLUME};

// Popup objects
static lv_obj_t* scale_popup	= NULL;
static lv_obj_t* step_popup	 = NULL;
static lv_obj_t* octave_display_label = NULL;
static lv_obj_t* bpm_popup	  = NULL;
static int       current_step_index = -1;
static lv_obj_t* scale_btn_label    = NULL;
static lv_obj_t* bpm_btn_label      = NULL;

// Step button references for updating note labels
static lv_obj_t* step_buttons[SEQUENCER_STEPS];
static lv_obj_t* step_button_labels[SEQUENCER_STEPS];

// Pattern button references
static lv_obj_t* pattern_buttons[PATTERN_COUNT];
static lv_obj_t* pattern_button_labels[PATTERN_COUNT];
static lv_obj_t* pattern_chain_indicators[PATTERN_COUNT];
static lv_obj_t* rand_button;
static lv_obj_t* density_slider;

// Forward declarations
static void update_step_button_display(int step_index);
static void update_all_step_buttons(void);
static void update_all_pattern_buttons(void);
static void step_update_timer_cb(lv_timer_t* timer);

// Timer for periodic step indicator updates
static lv_timer_t* step_update_timer = NULL;

static void bpm_keypad_event_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_READY) {
		lv_obj_t*   ta   = lv_event_get_target(e);
		const char* text = lv_textarea_get_text(ta);

		if (strlen(text) > 3) {
			return;
		}

		int bpm_value = atoi(text);
		if (bpm_value > 300) {
			bpm_value = 300;
		}

		synth_state_set_bpm((float)bpm_value, 44100.0f);

		// Update button label
		if (bpm_btn_label) {
			char bpm_text[16];
			snprintf(bpm_text, sizeof(bpm_text), "%d", bpm_value);
			lv_label_set_text(bpm_btn_label, bpm_text);
		}

		printf("BPM changed: %d\n", bpm_value);

		// Close popup
		if (bpm_popup) {
			lv_obj_del(bpm_popup);
			bpm_popup = NULL;
		}
	}
}

static void bpm_btn_event_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		if (bpm_popup) {
			lv_obj_del(bpm_popup);
			bpm_popup = NULL;
		} else {
			// Create BPM keypad popup
			bpm_popup = lv_obj_create(lv_scr_act());
			lv_obj_add_style(bpm_popup, &style_tab_container, 0);
			lv_obj_set_size(bpm_popup, 280, 280);
			lv_obj_center(bpm_popup);
			lv_obj_set_style_bg_opa(bpm_popup, LV_OPA_80, LV_PART_MAIN);

			// Title
			lv_obj_t* title = lv_label_create(bpm_popup);
			lv_label_set_text(title, "Enter BPM");
			lv_obj_add_style(title, &style_button_label, 0);
			lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

			// Text area for input
			lv_obj_t* ta = lv_textarea_create(bpm_popup);
			lv_textarea_set_max_length(ta, 3);
			lv_obj_set_size(ta, 160, 40);
			lv_obj_align(ta, LV_ALIGN_TOP_MID, 0, 40);
			lv_obj_set_style_bg_color(ta, lv_color_black(), LV_PART_MAIN);
			lv_obj_set_style_text_color(ta, lv_color_hex(0xAAAAAA), LV_PART_MAIN);
			lv_obj_set_style_border_color(ta, lv_color_hex(0x666666), LV_PART_MAIN);
			lv_obj_set_style_border_width(ta, 1, LV_PART_MAIN);
			char bpm_str[4];
			snprintf(bpm_str, sizeof(bpm_str), "%d", (int)synth_state_get_bpm());
			lv_textarea_set_text(ta, bpm_str);
			lv_textarea_set_one_line(ta, true);

			// Keypad
			lv_obj_t* kb = lv_keyboard_create(bpm_popup);
			lv_obj_set_style_bg_opa(bpm_popup, LV_OPA_60, LV_PART_MAIN);
			lv_obj_set_size(kb, 200, 180);
			lv_obj_align(kb, LV_ALIGN_TOP_MID, 0, 90);
			lv_keyboard_set_textarea(kb, ta);
			static const char* custom_map[] = {
			    "1", "2",  "3",	  "\n", "4",
			    "5", "6",  "\n",	 "7",  "8",
			    "9", "\n", LV_SYMBOL_OK, "0",  LV_SYMBOL_BACKSPACE,
			    "",
			};
			static const lv_btnmatrix_ctrl_t control_map[] = {
			    0, 0, 0, 0, 0, 0, 0, 0, 0, LV_BTNMATRIX_CTRL_CLICK_TRIG, 0, LV_BTNMATRIX_CTRL_CLICK_TRIG,
			};
			lv_keyboard_set_map(kb, LV_KEYBOARD_MODE_USER_1, custom_map, control_map);
			lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_USER_1);
			lv_obj_set_style_bg_color(kb, lv_color_black(), LV_PART_MAIN);
			lv_obj_set_style_text_color(kb, lv_color_hex(0xAAAAAA), LV_PART_MAIN);

			// Add event to handle text area changes
			lv_obj_add_event_cb(ta, bpm_keypad_event_cb, LV_EVENT_READY, NULL);
		}
	}
}

static void play_stop_event_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		lv_obj_t* btn = lv_event_get_target(e);

		// Toggle sequencer running state
		bool running = synth_state_is_running();

		// Update button appearance and text
		if (!running) {
			synth_state_start_sequencer();

			// Immediately update the UI to reflect the starting pattern
			update_all_step_buttons();
			update_all_pattern_buttons();
			lv_obj_set_style_bg_color(btn, lv_color_hex(0x00AA22), LV_PART_MAIN);
			lv_obj_t* label = lv_obj_get_child(btn, 0);
			if (label)
				lv_label_set_text(label, "STOP");

			// Start step indicator timer (update every 50ms for smooth indication)
			if (!step_update_timer) {
				step_update_timer = lv_timer_create(step_update_timer_cb, 50, NULL);
			}

			printf("Sequencer STARTED at %.1f BPM\n", synth_state_get_bpm());
		} else {
			synth_state_stop_sequencer();
			lv_obj_set_style_bg_color(btn, lv_color_hex(0x333333), LV_PART_MAIN);
			lv_obj_t* label = lv_obj_get_child(btn, 0);
			if (label)
				lv_label_set_text(label, "PLAY");

			// Stop step indicator timer
			if (step_update_timer) {
				lv_timer_del(step_update_timer);
				step_update_timer = NULL;
			}

			printf("Sequencer STOPPED\n");
		}

		// Immediately update step button displays
		update_all_step_buttons();
	}
}

static void close_scale_popup(lv_event_t* e)
{
	if (scale_popup) {
		lv_obj_del(scale_popup);
		scale_popup = NULL;
	}
}

static void scale_selected(lv_event_t* e)
{
	lv_obj_t* btn	 = lv_event_get_target(e);
	int       scale_index = (int)(intptr_t)lv_event_get_user_data(e);

	// Update scale in global state
	synth_state_set_scale(scale_index);

	// Update scale button label
	if (scale_btn_label) {
		lv_label_set_text(scale_btn_label, get_scale_name(synth_state_get_scale()));
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
		int editing_step = current_step_index;  // Remember which step was being edited
		lv_obj_del(step_popup);
		step_popup	 = NULL;
		octave_display_label = NULL;  // Clear octave display reference
		current_step_index = -1;
		
		// Update the step button to remove editing highlight
		if (editing_step >= 0) {
			update_step_button_display(editing_step);
		}
	}
}

static void accent_toggle_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		lv_obj_t* btn      = lv_event_get_target(e);
		int       step_idx = (int)(intptr_t)lv_event_get_user_data(e);

		// Toggle accent state in global state
		sequencer_step_t* step = synth_state_get_step(step_idx);
		if (step) {
			step->accent = !step->accent;

			// Update button appearance
			if (step->accent) {
				lv_obj_set_style_bg_color(btn, lv_color_hex(0x00AA22), LV_PART_MAIN);
				printf("Step %d: Accent ON\n", step_idx + 1);
			} else {
				lv_obj_set_style_bg_color(btn, lv_color_hex(0x333333), LV_PART_MAIN);
				printf("Step %d: Accent OFF\n", step_idx + 1);
			}
		}
	}
}

static void slide_toggle_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		lv_obj_t* btn      = lv_event_get_target(e);
		int       step_idx = (int)(intptr_t)lv_event_get_user_data(e);

		// Toggle slide state in global state
		sequencer_step_t* step = synth_state_get_step(step_idx);
		if (step) {
			step->slide = !step->slide;

			// Update button appearance
			if (step->slide) {
				lv_obj_set_style_bg_color(btn, lv_color_hex(0x00AA22), LV_PART_MAIN);
				printf("Step %d: Slide ON\n", step_idx + 1);
			} else {
				lv_obj_set_style_bg_color(btn, lv_color_hex(0x333333), LV_PART_MAIN);
				printf("Step %d: Slide OFF\n", step_idx + 1);
			}
		}
	}
}

static void octave_up_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		int		  step_idx = (int)(intptr_t)lv_event_get_user_data(e);
		sequencer_step_t* step     = synth_state_get_step(step_idx);
		if (step && step->octave < 2) {  // Limit to +2 octaves
			step->octave++;
			update_step_button_display(step_idx);
			
			// Update octave display in popup if it's open
			if (octave_display_label) {
				char octave_text[8];
				snprintf(octave_text, sizeof(octave_text), "%+d", step->octave);
				lv_label_set_text(octave_display_label, octave_text);
			}
			
			printf("Step %d: Octave +%d\n", step_idx + 1, step->octave);
		}
	}
}

static void octave_down_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		int		  step_idx = (int)(intptr_t)lv_event_get_user_data(e);
		sequencer_step_t* step     = synth_state_get_step(step_idx);
		if (step && step->octave > -2) {  // Limit to -2 octaves
			step->octave--;
			update_step_button_display(step_idx);
			
			// Update octave display in popup if it's open
			if (octave_display_label) {
				char octave_text[8];
				snprintf(octave_text, sizeof(octave_text), "%+d", step->octave);
				lv_label_set_text(octave_display_label, octave_text);
			}
			
			printf("Step %d: Octave %d\n", step_idx + 1, step->octave);
		}
	}
}

static void create_step_popup(int step_index)
{
	if (step_popup)
		return;

	current_step_index = step_index;
	step_popup	 = lv_obj_create(lv_scr_act());
	
	// Update the step button to show it's being edited
	update_step_button_display(step_index);
	lv_obj_set_size(step_popup, 200, 200);
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


	// Octave controls
	lv_obj_t* octave_label = lv_label_create(step_popup);
	lv_label_set_text(octave_label, "Octave:");
	lv_obj_add_style(octave_label, &style_button_label, 0);
	lv_obj_align(octave_label, LV_ALIGN_TOP_LEFT, 20, 40);

	// Octave down button
	lv_obj_t* oct_down_btn = lv_btn_create(step_popup);
	lv_obj_set_size(oct_down_btn, 30, 30);
	lv_obj_align(oct_down_btn, LV_ALIGN_TOP_LEFT, 90, 35);
	lv_obj_set_style_bg_color(oct_down_btn, lv_color_hex(0x333333), LV_PART_MAIN);
	lv_obj_set_style_border_color(oct_down_btn, lv_color_hex(0x666666), LV_PART_MAIN);
	lv_obj_set_style_border_width(oct_down_btn, 1, LV_PART_MAIN);
	lv_obj_set_style_radius(oct_down_btn, 2, LV_PART_MAIN);

	lv_obj_t* oct_down_label = lv_label_create(oct_down_btn);
	lv_label_set_text(oct_down_label, "-");
	lv_obj_add_style(oct_down_label, &style_button_label, 0);
	lv_obj_center(oct_down_label);
	lv_obj_add_event_cb(oct_down_btn, octave_down_cb, LV_EVENT_CLICKED, (void*)(intptr_t)step_index);

	// Octave display
	octave_display_label = lv_label_create(step_popup);
	sequencer_step_t* current_step = synth_state_get_step(step_index);
	char		  octave_text[8];
	snprintf(octave_text, sizeof(octave_text), "%+d", current_step ? current_step->octave : 0);
	lv_label_set_text(octave_display_label, octave_text);
	lv_obj_add_style(octave_display_label, &style_button_label, 0);
	lv_obj_align(octave_display_label, LV_ALIGN_TOP_LEFT, 135, 45);

	// Octave up button
	lv_obj_t* oct_up_btn = lv_btn_create(step_popup);
	lv_obj_set_size(oct_up_btn, 30, 30);
	lv_obj_align(oct_up_btn, LV_ALIGN_TOP_LEFT, 160, 35);
	lv_obj_set_style_bg_color(oct_up_btn, lv_color_hex(0x333333), LV_PART_MAIN);
	lv_obj_set_style_border_color(oct_up_btn, lv_color_hex(0x666666), LV_PART_MAIN);
	lv_obj_set_style_border_width(oct_up_btn, 1, LV_PART_MAIN);
	lv_obj_set_style_radius(oct_up_btn, 2, LV_PART_MAIN);

	lv_obj_t* oct_up_label = lv_label_create(oct_up_btn);
	lv_label_set_text(oct_up_label, "+");
	lv_obj_add_style(oct_up_label, &style_button_label, 0);
	lv_obj_center(oct_up_label);
	lv_obj_add_event_cb(oct_up_btn, octave_up_cb, LV_EVENT_CLICKED, (void*)(intptr_t)step_index);

	// Accent toggle button and label
	lv_obj_t* accent_btn = lv_btn_create(step_popup);
	lv_obj_set_size(accent_btn, 20, 20);
	lv_obj_align(accent_btn, LV_ALIGN_TOP_LEFT, 20, 80);
	// Style button based on accent state
	if (current_step && current_step->accent) {
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
	lv_obj_align(accent_label, LV_ALIGN_TOP_LEFT, 50, 85);

	// Add event callback for accent button
	lv_obj_add_event_cb(accent_btn, accent_toggle_cb, LV_EVENT_CLICKED, (void*)(intptr_t)step_index);

	// Slide toggle button and label
	lv_obj_t* slide_btn = lv_btn_create(step_popup);
	lv_obj_set_size(slide_btn, 20, 20);
	lv_obj_align(slide_btn, LV_ALIGN_TOP_LEFT, 20, 120);
	// Style button based on slide state
	if (current_step && current_step->slide) {
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
	lv_obj_align(slide_label, LV_ALIGN_TOP_LEFT, 50, 125);

	// Add event callback for slide button
	lv_obj_add_event_cb(slide_btn, slide_toggle_cb, LV_EVENT_CLICKED, (void*)(intptr_t)step_index);

	// Add click outside to close functionality
	lv_obj_add_flag(step_popup, LV_OBJ_FLAG_CLICK_FOCUSABLE);
	lv_obj_add_event_cb(lv_scr_act(), close_step_popup, LV_EVENT_CLICKED, NULL);
}

// Flag to prevent click after long press
static bool long_press_occurred = false;

// Drag state for note selection
static bool     drag_mode		  = false;
static int      drag_start_y		  = 0;
static int      current_drag_step	 = -1;
static uint32_t long_press_start_time     = 0;
static bool     note_changed_during_press = false;

static void update_step_button_display(int step_index)
{
	if (step_index < 0 || step_index >= SEQUENCER_STEPS)
		return;

	sequencer_step_t* step = synth_state_get_step(step_index);
	if (!step || !step_buttons[step_index] || !step_button_labels[step_index])
		return;

	// Update button color based on state priority: playing > editing > active > inactive
	// Only show the current step indicator for the currently active/playing pattern
	lv_color_t bg_color;
	if (synth_state_is_running() && step_index == synth_state_get_current_step_index() &&
	    synth_state_get_current_pattern() == synth_state_get_active_pattern()) {
		// Currently playing step on the active pattern - bright theme color
		bg_color = lv_color_hex(0x00FF44);  // Bright green for current step
	} else if (step_index == current_step_index && step_popup) {
		// Currently being edited (popup is open) - highlight in blue
		bg_color = lv_color_hex(0x0066AA);  // Blue for editing
	} else if (step->active) {
		// Active step but not currently playing or editing
		bg_color = lv_color_hex(0x00AA22);  // Normal green for active steps
	} else {
		// Inactive step
		bg_color = lv_color_hex(0x333333);  // Dark gray for inactive steps
	}
	lv_obj_set_style_bg_color(step_buttons[step_index], bg_color, LV_PART_MAIN);

	// Update note label with octave information
	const int*  scale_intervals = get_scale_intervals(synth_state_get_scale());
	const char* note_name       = "C";
	if (scale_intervals && step->note_index >= 0 && step->note_index < 8) {
		int note  = scale_intervals[step->note_index] % 12;
		note_name = get_note_name(note);
	}
	
	// Include octave information if it's not 0
	char label_text[8];
	if (step->octave != 0) {
		snprintf(label_text, sizeof(label_text), "%s%+d", note_name, step->octave);
	} else {
		snprintf(label_text, sizeof(label_text), "%s", note_name);
	}
	lv_label_set_text(step_button_labels[step_index], label_text);
}

// Update all step button displays (for current step indicator refresh)
static void update_all_step_buttons(void)
{
	for (int i = 0; i < SEQUENCER_STEPS; i++) {
		update_step_button_display(i);
	}
}

static void step_update_timer_cb(lv_timer_t* timer)
{
	if (synth_state_is_running()) {
		update_all_step_buttons();
		update_all_pattern_buttons();  // Update pattern highlighting when sequencer is running
	}
}

// Pattern selection and chaining variables
static int      pattern_long_press_index    = -1;
static uint32_t pattern_long_press_start    = 0;
static bool     pattern_long_press_occurred = false;

// Update pattern button display based on state
static void update_pattern_button_display(int pattern_index)
{
	if (pattern_index < 0 || pattern_index >= PATTERN_COUNT)
		return;
	if (!pattern_buttons[pattern_index] || !pattern_button_labels[pattern_index] ||
	    !pattern_chain_indicators[pattern_index])
		return;

	// Update button background color with priority: playing > selected > not selected
	lv_color_t bg_color;
	bool is_current_pattern = (pattern_index == synth_state_get_current_pattern());
	bool is_active_pattern = (synth_state_is_running() && pattern_index == synth_state_get_active_pattern());
	
	if (is_active_pattern) {
		// Currently playing pattern - bright highlight (similar to step highlight)
		bg_color = lv_color_hex(0x00FF44);  // Bright green for playing
	} else if (is_current_pattern) {
		// Currently selected for editing but not playing
		bg_color = lv_color_hex(0x00AA22);  // Normal green for selected
	} else {
		// Not selected or playing
		bg_color = lv_color_hex(0x333333);  // Dark gray for inactive
	}
	lv_obj_set_style_bg_color(pattern_buttons[pattern_index], bg_color, LV_PART_MAIN);

	// Update chain indicator color
	lv_color_t indicator_color;
	if (synth_state_is_pattern_chained(pattern_index)) {
		// Pattern is in chain - green indicator
		indicator_color = lv_color_hex(0x00AA22);
	} else {
		// Pattern not in chain - gray indicator
		indicator_color = lv_color_hex(0x666666);
	}
	lv_obj_set_style_bg_color(pattern_chain_indicators[pattern_index], indicator_color, LV_PART_MAIN);
}

// Update all pattern button displays
static void update_all_pattern_buttons(void)
{
	for (int i = 0; i < PATTERN_COUNT; i++) {
		update_pattern_button_display(i);
	}
}

// Randomize a pattern with steps and notes using density slider
static void randomize_pattern(int pattern_index)
{
	if (pattern_index < 0 || pattern_index >= PATTERN_COUNT) {
		return;
	}

	// Seed random number generator (using current time for variety)
	static bool seeded = false;
	if (!seeded) {
		srand((unsigned int)lv_tick_get());
		seeded = true;
	}

	// Get density from slider (30-100%)
	int density_percent = lv_slider_get_value(density_slider);
	printf("Randomizing pattern %d with %d%% density\n", pattern_index + 1, density_percent);

	// Randomize each step
	for (int step = 0; step < SEQUENCER_STEPS; step++) {
		sequencer_step_t* step_data = synth_state_get_step_from_pattern(pattern_index, step);
		if (!step_data) continue;

		// Use density slider value for chance of step being active
		step_data->active = (rand() % 100) < density_percent;

		if (step_data->active) {
			// Random note in scale (0-7)
			step_data->note_index = rand() % 8;

			// 20% chance for accent
			step_data->accent = (rand() % 100) < 20;

			// 10% chance for slide
			step_data->slide = (rand() % 100) < 10;
		} else {
			// Clear inactive step properties
			step_data->note_index = 0;
			step_data->octave = 0;
			step_data->accent = false;
			step_data->slide = false;
		}
	}

	// Update UI to show changes
	update_all_step_buttons();
	printf("Pattern %d randomized\n", pattern_index + 1);
}

static void rand_button_event_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		int current_pattern = synth_state_get_current_pattern();
		randomize_pattern(current_pattern);
	}
}

static void pattern_button_event_cb(lv_event_t* e)
{
	lv_event_code_t code	  = lv_event_get_code(e);
	int		pattern_index = (int)(intptr_t)lv_event_get_user_data(e);

	if (code == LV_EVENT_LONG_PRESSED) {
		// Start pattern chaining mode
		pattern_long_press_index    = pattern_index;
		pattern_long_press_start    = lv_tick_get();
		pattern_long_press_occurred = true;
		printf("Pattern %d: Long press - chaining mode active (press other patterns to chain)\n",
		       pattern_index + 1);

	} else if (code == LV_EVENT_PRESSED) {
		// Check if we're in chaining mode and this is a different pattern
		// Using PRESSED instead of HOVER_OVER for more reliable detection
		if (pattern_long_press_index >= 0 && pattern_long_press_index != pattern_index &&
		    (lv_tick_get() - pattern_long_press_start) < 3000) {
			printf("Pattern %d: Detected chaining attempt with pattern %d\n", pattern_long_press_index + 1,
			       pattern_index + 1);

			// Handle chaining logic on hover/press
			bool first_chained  = synth_state_is_pattern_chained(pattern_long_press_index);
			bool second_chained = synth_state_is_pattern_chained(pattern_index);

			if (first_chained && second_chained) {
				// Both chained - remove the first one from chain
				synth_state_unchain_pattern(pattern_long_press_index);
				printf("Pattern %d: Removed from chain\n", pattern_long_press_index + 1);
			} else if (!first_chained && !second_chained) {
				// Neither chained - chain both
				synth_state_chain_pattern(pattern_long_press_index);
				synth_state_chain_pattern(pattern_index);
				printf("Pattern %d and %d: Chained together\n", pattern_long_press_index + 1,
				       pattern_index + 1);
			} else {
				// One chained, one not - add the unchained one to chain
				if (!first_chained)
					synth_state_chain_pattern(pattern_long_press_index);
				if (!second_chained)
					synth_state_chain_pattern(pattern_index);
				printf("Pattern %d: Added to existing chain\n",
				       (!first_chained ? pattern_long_press_index : pattern_index) + 1);
			}

			update_all_pattern_buttons();

			// Reset chaining mode after chaining operation
			pattern_long_press_index    = -1;
			pattern_long_press_occurred = false;
			return;
		}

	} else if (code == LV_EVENT_CLICKED) {
		// Don't process click if it's from a long press release
		if (pattern_long_press_occurred && pattern_index == pattern_long_press_index) {
			pattern_long_press_occurred = false;  // Reset flag but stay in chaining mode
			printf("Pattern %d: Ignoring click after long press\n", pattern_index + 1);
			return;
		}

		// Reset chaining mode if clicking outside chaining window
		if (pattern_long_press_index >= 0 && (lv_tick_get() - pattern_long_press_start) >= 3000) {
			pattern_long_press_index    = -1;
			pattern_long_press_occurred = false;
		}

		// Normal pattern selection (if not in chaining mode)
		if (pattern_long_press_index < 0) {
			// Always allow switching patterns for editing
			synth_state_set_current_pattern(pattern_index);
			update_all_pattern_buttons();
			update_all_step_buttons();  // Refresh step buttons for new pattern
			printf("Pattern %d: Selected\n", pattern_index + 1);
		}
	}
}

static void step_button_event_cb(lv_event_t* e)
{
	lv_event_code_t code       = lv_event_get_code(e);
	lv_obj_t*       btn	= (lv_obj_t*)lv_event_get_target(e);
	int		step_index = (int)(intptr_t)lv_event_get_user_data(e);

	if (code == LV_EVENT_LONG_PRESSED) {
		// Enter drag mode for note selection and start timing
		long_press_occurred       = true;
		drag_mode		  = true;
		current_drag_step	 = step_index;
		long_press_start_time     = lv_tick_get();
		note_changed_during_press = false;

		// Get initial Y position
		lv_indev_t* indev = lv_indev_get_act();
		if (indev) {
			lv_point_t point;
			lv_indev_get_point(indev, &point);
			drag_start_y = point.y;
		}

		printf("Step %d: Long press - entering drag mode\n", step_index + 1);

	} else if (code == LV_EVENT_PRESSING && drag_mode && current_drag_step == step_index) {
		// Check if 1 second has passed without note change - open popup first
		if (!note_changed_during_press && (lv_tick_get() - long_press_start_time) > 1000) {
			drag_mode	   = false;  // Exit drag mode
			current_drag_step   = -1;
			long_press_occurred = false;  // Reset to allow popup interaction
			create_step_popup(step_index);
			printf("Step %d: Opening popup after 1-second delay\n", step_index + 1);
			return;  // Exit early to prevent drag processing
		}

		// Handle drag for note selection
		lv_indev_t* indev = lv_indev_get_act();
		if (indev) {
			lv_point_t point;
			lv_indev_get_point(indev, &point);
			int drag_delta = drag_start_y - point.y;  // Negative = drag down, positive = drag up

			// Calculate note change based on drag distance (every 8 pixels = 1 note, more sensitive)
			int note_change = drag_delta / 8;

			sequencer_step_t* step = synth_state_get_step(step_index);
			if (step) {
				const int* scale_intervals = get_scale_intervals(synth_state_get_scale());
				int	scale_length    = 0;
				while (scale_length < 8 && scale_intervals[scale_length] >= 0)
					scale_length++;

				int new_note_index = step->note_index + note_change;

				// Clamp to scale range
				if (new_note_index < 0)
					new_note_index = 0;
				if (new_note_index >= scale_length)
					new_note_index = scale_length - 1;

				if (new_note_index != step->note_index) {
					step->note_index	  = new_note_index;
					note_changed_during_press = true;  // Mark that note changed
					update_step_button_display(step_index);

					const char* note_name = get_note_name(scale_intervals[new_note_index] % 12);
					printf("Step %d: Note changed to %s\n", step_index + 1, note_name);

					// Update drag start position to prevent continuous jumping
					drag_start_y = point.y;
				}
			}
		}

	} else if (code == LV_EVENT_RELEASED) {
		// Exit drag mode
		if (drag_mode) {
			drag_mode	 = false;
			current_drag_step = -1;
			printf("Step %d: Released - exiting drag mode\n", step_index + 1);
		}

	} else if (code == LV_EVENT_CLICKED) {
		// Only toggle if it wasn't a long press or drag
		if (long_press_occurred || drag_mode) {
			long_press_occurred = false;  // Reset flag
			return;			      // Don't toggle on long press or drag
		}

		// Toggle step state in global state
		sequencer_step_t* step = synth_state_get_step(step_index);
		if (step) {
			step->active = !step->active;
			update_step_button_display(step_index);
			printf("Step %d: %s\n", step_index + 1, step->active ? "ON" : "OFF");
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
	const char* names[] = {"PAR", "SEQ", "OSC", "SET"};
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

	// Pattern buttons container - PAT buttons aligned with scale button center
	// Scale button center: x=50 (x=10, width=80)
	// PAT buttons: width=45, so to center at x=50, they start at x=27.5
	// Indicators: width=8 + gap=3 = 11px before button
	// Container starts at x=16.5 so buttons center aligns with scale button
	lv_obj_t* pattern_container = lv_obj_create(sequencer_screen);
	lv_obj_add_style(pattern_container, &style_tab_container, 0);
	lv_obj_set_size(pattern_container, 70, 240);
	lv_obj_align(pattern_container, LV_ALIGN_LEFT_MID, 17, 0);  // x=17 for PAT button alignment
	lv_obj_set_flex_flow(pattern_container, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(pattern_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_pad_all(pattern_container, 5, 0);
	lv_obj_set_style_pad_gap(pattern_container, 8, 0);

	// Create 4 pattern buttons (PAT1-PAT4)
	const char* pattern_names[] = {"PAT1", "PAT2", "PAT3", "PAT4"};
	for (int i = 0; i < PATTERN_COUNT; i++) {
		// Create button container to hold both button and indicator
		lv_obj_t* btn_container = lv_obj_create(pattern_container);
		lv_obj_add_style(btn_container, &style_tab_container, 0);
		lv_obj_set_size(btn_container, 60, 40);
		lv_obj_set_layout(btn_container, LV_LAYOUT_FLEX);
		lv_obj_set_flex_flow(btn_container, LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(btn_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
		lv_obj_set_style_pad_all(btn_container, 2, 0);
		lv_obj_set_style_pad_gap(btn_container, 3, 0);

		// Create circular chain indicator on the left
		lv_obj_t* indicator	 = lv_obj_create(btn_container);
		pattern_chain_indicators[i] = indicator;
		lv_obj_set_size(indicator, 8, 8);
		lv_obj_set_style_radius(indicator, LV_RADIUS_CIRCLE, LV_PART_MAIN);
		lv_obj_set_style_border_width(indicator, 0, LV_PART_MAIN);
		// Initial color - gray for not chained
		lv_obj_set_style_bg_color(indicator, lv_color_hex(0x666666), LV_PART_MAIN);

		// Create the main pattern button
		lv_obj_t* btn = lv_btn_create(btn_container);
		lv_obj_set_size(btn, 45, 36);

		// Store reference to button
		pattern_buttons[i] = btn;

		// Style the button
		lv_color_t bg_color =
		    (i == synth_state_get_current_pattern()) ? lv_color_hex(0x00AA22) : lv_color_hex(0x333333);
		lv_obj_set_style_bg_color(btn, bg_color, LV_PART_MAIN);
		lv_obj_set_style_border_color(btn, lv_color_hex(0x666666), LV_PART_MAIN);
		lv_obj_set_style_border_width(btn, 1, LV_PART_MAIN);
		lv_obj_set_style_radius(btn, 3, LV_PART_MAIN);

		// Add pattern label
		lv_obj_t* label		 = lv_label_create(btn);
		pattern_button_labels[i] = label;
		lv_label_set_text(label, pattern_names[i]);
		lv_obj_set_style_text_color(label, lv_color_black(), LV_PART_MAIN);
		// Use default font size for pattern labels
		lv_obj_center(label);

		// Add event callbacks
		lv_obj_add_event_cb(btn, pattern_button_event_cb, LV_EVENT_CLICKED, (void*)(intptr_t)i);
		lv_obj_add_event_cb(btn, pattern_button_event_cb, LV_EVENT_LONG_PRESSED, (void*)(intptr_t)i);
		lv_obj_add_event_cb(btn, pattern_button_event_cb, LV_EVENT_PRESSED, (void*)(intptr_t)i);
	}

	// Grid container centered on screen (with pattern buttons on left balancing transport on right)
	lv_obj_t* grid_container = lv_obj_create(sequencer_screen);
	lv_obj_add_style(grid_container, &style_tab_container, 0);
	lv_obj_set_size(grid_container, 240, 240);
	lv_obj_center(grid_container);  // Center the grid
	lv_obj_set_layout(grid_container, LV_LAYOUT_GRID);
	lv_obj_set_style_pad_all(grid_container, 8, 0);
	lv_obj_set_style_pad_gap(grid_container, 4, 0);

	// Define grid: 4 columns, 4 rows
	static int32_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
	static int32_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
	lv_obj_set_grid_dsc_array(grid_container, col_dsc, row_dsc);

	// Create 4x4 grid of buttons (16 total)
	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++) {
			int step_index = row * 4 + col;

			lv_obj_t* btn = lv_btn_create(grid_container);
			lv_obj_set_size(btn, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
			lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_STRETCH, col, 1, LV_GRID_ALIGN_STRETCH, row, 1);

			// Store reference to button
			step_buttons[step_index] = btn;

			// Style the button to be square
			sequencer_step_t* step = synth_state_get_step(step_index);
			lv_color_t bg_color = (step && step->active) ? lv_color_hex(0x00AA22) : lv_color_hex(0x333333);
			lv_obj_set_style_bg_color(btn, bg_color, LV_PART_MAIN);
			lv_obj_set_style_border_color(btn, lv_color_hex(0x666666), LV_PART_MAIN);
			lv_obj_set_style_border_width(btn, 1, LV_PART_MAIN);
			lv_obj_set_style_radius(btn, 2, LV_PART_MAIN);

			// Add note label to button
			lv_obj_t* label		       = lv_label_create(btn);
			step_button_labels[step_index] = label;
			const int*  scale_intervals    = get_scale_intervals(synth_state_get_scale());
			const char* note_name	  = "C";
			if (step && scale_intervals && step->note_index >= 0 && step->note_index < 8) {
				int note  = scale_intervals[step->note_index] % 12;
				note_name = get_note_name(note);
			}
			lv_label_set_text(label, note_name);
			lv_obj_set_style_text_color(label, lv_color_black(), LV_PART_MAIN);
			lv_obj_center(label);

			// Add event callbacks for click, long press, and drag
			lv_obj_add_event_cb(btn, step_button_event_cb, LV_EVENT_CLICKED, (void*)(intptr_t)step_index);
			lv_obj_add_event_cb(btn, step_button_event_cb, LV_EVENT_LONG_PRESSED,
					    (void*)(intptr_t)step_index);
			lv_obj_add_event_cb(btn, step_button_event_cb, LV_EVENT_PRESSING, (void*)(intptr_t)step_index);
			lv_obj_add_event_cb(btn, step_button_event_cb, LV_EVENT_RELEASED, (void*)(intptr_t)step_index);
		}
	}

	// Scale button in top left corner
	lv_obj_t* scale_btn = lv_btn_create(sequencer_screen);
	lv_obj_add_style(scale_btn, &style_scale_button, 0);
	lv_obj_set_size(scale_btn, 80, 30);
	lv_obj_align(scale_btn, LV_ALIGN_TOP_LEFT, 10, 10);
	scale_btn_label = lv_label_create(scale_btn);
	lv_label_set_text(scale_btn_label, get_scale_name(synth_state_get_scale()));
	lv_obj_add_style(scale_btn_label, &style_step_label, 0);
	lv_obj_center(scale_btn_label);
	lv_obj_add_event_cb(scale_btn, scale_btn_event_cb, LV_EVENT_CLICKED, NULL);

	// Transport controls container - positioned to the right to balance pattern buttons on left
	lv_obj_t* transport_cont = lv_obj_create(sequencer_screen);
	lv_obj_add_style(transport_cont, &style_tab_container, 0);
	lv_obj_set_size(transport_cont, 70, 200);
	lv_obj_align(transport_cont, LV_ALIGN_RIGHT_MID, -5, 0);  // Right side with small margin
	lv_obj_set_flex_flow(transport_cont, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(transport_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

	// Play/Stop button - smaller to fit balanced layout
	lv_obj_t* play_btn = lv_btn_create(transport_cont);
	lv_obj_add_style(play_btn, &style_button, 0);
	lv_obj_set_size(play_btn, 60, 35);
	lv_obj_set_style_bg_color(play_btn, lv_color_hex(0x333333), LV_PART_MAIN);
	lv_obj_set_style_border_color(play_btn, lv_color_hex(0x666666), LV_PART_MAIN);
	lv_obj_set_style_border_width(play_btn, 1, LV_PART_MAIN);
	lv_obj_t* play_label = lv_label_create(play_btn);
	lv_label_set_text(play_label, "PLAY");
	lv_obj_add_style(play_label, &style_button_label_black, 0);
	// Use default font for play button
	lv_obj_center(play_label);
	lv_obj_add_event_cb(play_btn, play_stop_event_cb, LV_EVENT_CLICKED, NULL);



	// Single RAND button
	rand_button = lv_btn_create(transport_cont);
	lv_obj_set_size(rand_button, 60, 35);
	lv_obj_set_style_bg_color(rand_button, lv_color_hex(0x333333), LV_PART_MAIN);
	lv_obj_set_style_border_color(rand_button, lv_color_hex(0x666666), LV_PART_MAIN);
	lv_obj_set_style_border_width(rand_button, 1, LV_PART_MAIN);
	lv_obj_set_style_radius(rand_button, 2, LV_PART_MAIN);

	lv_obj_t* rand_label = lv_label_create(rand_button);
	lv_label_set_text(rand_label, "RAND");
	lv_obj_add_style(rand_label, &style_button_label_black, 0);
	lv_obj_center(rand_label);
	lv_obj_add_event_cb(rand_button, rand_button_event_cb, LV_EVENT_CLICKED, NULL);

	// Vertical density slider (30-100%)
	density_slider = lv_slider_create(transport_cont);
	lv_obj_set_size(density_slider, 15, 80);
	lv_slider_set_range(density_slider, 30, 100);
	lv_slider_set_value(density_slider, 60, LV_ANIM_OFF);
	lv_obj_add_style(density_slider, &style_slider, LV_PART_MAIN);
	lv_obj_add_style(density_slider, &style_slider_indicator, LV_PART_INDICATOR);
	lv_obj_add_style(density_slider, &style_slider_knob, LV_PART_KNOB);

	// DNS label below slider
	lv_obj_t* dns_label = lv_label_create(transport_cont);
	lv_label_set_text(dns_label, "DNS");
	lv_obj_add_style(dns_label, &style_button_label, 0);

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

static lv_obj_t* create_settings_screen(void)
{
	settings_screen = lv_obj_create(NULL);
	lv_obj_add_style(settings_screen, &style_screen, 0);

	// Main container for settings
	lv_obj_t* main_cont = lv_obj_create(settings_screen);
	lv_obj_add_style(main_cont, &style_tab_container, 0);
	lv_obj_set_size(main_cont, lv_pct(80), lv_pct(80));
	lv_obj_center(main_cont);
	lv_obj_set_flex_flow(main_cont, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(main_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

	// Title
	lv_obj_t* title = lv_label_create(main_cont);
	lv_label_set_text(title, "SETTINGS");
	lv_obj_add_style(title, &style_button_label, 0);

	// BPM button
	lv_obj_t* bpm_btn = lv_btn_create(main_cont);
	lv_obj_set_size(bpm_btn, 120, 40);
	lv_obj_set_style_bg_color(bpm_btn, lv_color_black(), LV_PART_MAIN);
	lv_obj_set_style_border_color(bpm_btn, lv_color_hex(0x666666), LV_PART_MAIN);
	lv_obj_set_style_border_width(bpm_btn, 1, LV_PART_MAIN);
	lv_obj_set_style_radius(bpm_btn, 2, LV_PART_MAIN);
	bpm_btn_label = lv_label_create(bpm_btn);
	char bpm_text[16];
	snprintf(bpm_text, sizeof(bpm_text), "%d BPM", (int)synth_state_get_bpm());
	lv_label_set_text(bpm_btn_label, bpm_text);
	lv_obj_add_style(bpm_btn_label, &style_button_label, 0);
	lv_obj_center(bpm_btn_label);
	lv_obj_add_event_cb(bpm_btn, bpm_btn_event_cb, LV_EVENT_CLICKED, NULL);

	// Menu
	lv_obj_t* menu = create_menu_flex(settings_screen);
	create_menu_buttons(menu);

	return settings_screen;
}

// --------------------------
// UI init
// --------------------------
void ui_init(lv_display_t* disp)
{
	// Initialize global synthesizer state
	synth_state_init();

	init_styles();

	params_screen    = create_params_screen();
	sequencer_screen = create_seq_screen();
	settings_screen  = create_settings_screen();

	// Initialize pattern button displays
	update_all_pattern_buttons();

	lv_scr_load(params_screen);
}
