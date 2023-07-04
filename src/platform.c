/* Includes ----------------------------------------------------------------- */

#include <time.h>
#include <stdio.h>

#include "platform.h"
#include "constants.h"

#include "raylib.h"

/* Global variables --------------------------------------------------------- */

static volatile uint32_t init_clock;

/* Function definitions ----------------------------------------------------- */

void platform_screen_init(void)
{
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Doom Pico");
	SetTargetFPS(FPS);
}

void platform_screen_draw_start(void)
{
	BeginDrawing();
}

void platform_screen_draw_stop(void)
{
	EndDrawing();
}

void platform_screen_draw_pixel(uint8_t x, uint8_t y, bool color)
{
	DrawPixel(x, y, color ? WHITE : BLACK);
}

void platform_audio_init(void)
{
}

void platform_audio_play_start(void)
{
}

void platform_audio_play_stop(void)
{
}

void platform_utils_init(void)
{
	init_clock = clock();
}

uint32_t platform_utils_millis(void)
{
	return ((clock() - init_clock) * 1000) / CLOCKS_PER_SEC;
}

void platform_utils_delay(uint32_t ms)
{
	uint32_t t0 = platform_utils_millis();
	while ((platform_utils_millis() - t0) < ms)
	{
		asm("nop");
	};
}

/* -------------------------------------------------------------------------- */