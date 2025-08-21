#include "ui_backend.h"
#include "lvgl.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static lv_color_t    buf1[LV_HOR_RES * 40];  // buffer for partial rendering
static lv_display_t* disp;

static SDL_Window*   window   = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture*  texture  = NULL;

/* Flush callback */
static void flush_cb(lv_display_t* d, const lv_area_t* area, uint8_t* color_p)
{
	lv_color_t* buf = (lv_color_t*)color_p;
	SDL_UpdateTexture(texture, NULL, buf, LV_HOR_RES * sizeof(lv_color_t));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
	lv_display_flush_ready(d);
}

/* Input callback */
static void touch_read_cb(lv_indev_t* indev, lv_indev_data_t* data)
{
	SDL_Event event;
	data->state   = LV_INDEV_STATE_RELEASED;
	data->point.x = 0;
	data->point.y = 0;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT)
			exit(0);
		else if (event.type == SDL_MOUSEBUTTONDOWN) {
			data->state   = LV_INDEV_STATE_PRESSED;
			data->point.x = event.button.x;
			data->point.y = event.button.y;
		} else if (event.type == SDL_MOUSEBUTTONUP) {
			data->state = LV_INDEV_STATE_RELEASED;
		} else if (event.type == SDL_MOUSEMOTION && data->state == LV_INDEV_STATE_PRESSED) {
			data->point.x = event.motion.x;
			data->point.y = event.motion.y;
		}
	}
}

/* Initialize SDL backend */
lv_display_t* ui_backend_init(void)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
		fprintf(stderr, "SDL init failed: %s\n", SDL_GetError());
		exit(1);
	}

	window = SDL_CreateWindow("LVGL SDL Backend", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, LV_HOR_RES,
				  LV_VER_RES, 0);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	texture	 = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, LV_HOR_RES,
				     LV_VER_RES);

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
