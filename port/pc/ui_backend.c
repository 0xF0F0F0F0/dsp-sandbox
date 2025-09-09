#include "ui_backend.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
	int  x;
	int  y;
	bool pressed;
} mouse_state_t;

static mouse_state_t mouse_state = {0, 0, false};

static lv_color16_t  buf1[LV_HOR_RES * 40];  // buffer for partial rendering
static lv_display_t* disp;

static SDL_Window*   window   = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture*  texture  = NULL;

/* Flush callback */
static void flush_cb(lv_display_t* d, const lv_area_t* area, uint8_t* color_p)
{
	lv_color16_t* buf = (lv_color16_t*)color_p;

	SDL_Rect sdl_area = {.x = area->x1, .y = area->y1, .w = lv_area_get_width(area), .h = lv_area_get_height(area)};

	int pitch = lv_area_get_width(area) * sizeof(lv_color16_t);

	SDL_UpdateTexture(texture, &sdl_area, buf, pitch);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

	lv_display_flush_ready(d);
}

/* Poll events once per loop */
void poll_sdl_events(void)
{
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) {
			exit(0);
		} else if (e.type == SDL_MOUSEBUTTONDOWN) {
			mouse_state.pressed = true;
			mouse_state.x       = e.button.x;
			mouse_state.y       = e.button.y;
		} else if (e.type == SDL_MOUSEBUTTONUP) {
			mouse_state.pressed = false;
		} else if (e.type == SDL_MOUSEMOTION) {
			mouse_state.x = e.motion.x;
			mouse_state.y = e.motion.y;
		}
	}
}

/* LVGL read callback: just return current state */
static void touch_read_cb(lv_indev_t* indev, lv_indev_data_t* data)
{
	data->state   = mouse_state.pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
	data->point.x = mouse_state.x;
	data->point.y = mouse_state.y;
}

/* Initialize SDL backend */
lv_display_t* ui_backend_init(void)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
		fprintf(stderr, "SDL init failed: %s\n", SDL_GetError());
		exit(1);
	}

	int scale = 1;
	if (LV_HOR_RES <= 480) {
		scale = 2;
	}

	window = SDL_CreateWindow("LVGL SDL Backend", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				  LV_HOR_RES * scale, LV_VER_RES * scale, 0);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderSetLogicalSize(renderer, LV_HOR_RES, LV_VER_RES);
	texture =
	    SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, LV_HOR_RES, LV_VER_RES);

	if (!window || !renderer || !texture) {
		fprintf(stderr, "SDL initialization failed\n");
		exit(1);
	}

	disp = lv_display_create(LV_HOR_RES, LV_VER_RES);
	lv_display_set_flush_cb(disp, flush_cb);
	lv_display_set_buffers(disp, buf1, NULL, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);

	lv_indev_t* mouse = lv_indev_create();
	lv_indev_set_type(mouse, LV_INDEV_TYPE_POINTER);
	lv_indev_set_read_cb(mouse, touch_read_cb);

	return disp;
}

void ui_backend_cleanup(void)
{
	// SDL cleanup is handled by the platform layer
}
