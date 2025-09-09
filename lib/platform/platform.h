#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdbool.h>

// Platform initialization interface
typedef struct {
	bool (*init)(void);
	void (*cleanup)(void);
	void (*delay_ms)(int ms);
	unsigned int (*get_ticks)(void);
} platform_t;

// Platform instance (implemented by each platform)
extern const platform_t* platform;

// Platform initialization
bool	 platform_init(void);
void	 platform_cleanup(void);
void	 platform_delay_ms(int ms);
unsigned int platform_get_ticks(void);

#endif  // PLATFORM_H
