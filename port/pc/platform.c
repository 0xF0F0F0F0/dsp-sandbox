#include "platform.h"
#include "ui_backend.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

static bool pc_init(void)
{
	return SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == 0;
}

static void pc_cleanup(void)
{
	ui_backend_cleanup();
	SDL_Quit();
}

static void pc_delay_ms(int ms)
{
	SDL_Delay(ms);
}

static unsigned int pc_get_ticks(void)
{
	return SDL_GetTicks();
}

// Platform implementation for PC
static const platform_t pc_platform = {.init      = pc_init,
				       .cleanup   = pc_cleanup,
				       .delay_ms  = pc_delay_ms,
				       .get_ticks = pc_get_ticks};

const platform_t* platform = &pc_platform;

// Platform interface functions
bool platform_init(void)
{
	return platform->init();
}

void platform_cleanup(void)
{
	platform->cleanup();
}

void platform_delay_ms(int ms)
{
	platform->delay_ms(ms);
}

unsigned int platform_get_ticks(void)
{
	return platform->get_ticks();
}