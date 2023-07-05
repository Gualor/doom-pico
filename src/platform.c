/* Includes ----------------------------------------------------------------- */

#include <time.h>
#include <stdio.h>

#include "platform.h"
#include "constants.h"

#include "raylib.h"

/* Global variables --------------------------------------------------------- */

static volatile uint32_t init_clock;

/* Function definitions ----------------------------------------------------- */

void platform_init(void)
{
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Doom Pico");
	SetTargetFPS(FPS);

	init_clock = clock();
}

void platform_draw_start(void)
{
	BeginDrawing();
}

void platform_draw_stop(void)
{
	EndDrawing();
}

void platform_draw_pixel(uint8_t x, uint8_t y, bool color)
{
	DrawPixel(x, y, color ? WHITE : BLACK);
}

void platform_play_audio(bool play)
{
}

uint32_t platform_millis(void)
{
	return ((clock() - init_clock) * 1000) / CLOCKS_PER_SEC;
}

void platform_delay(uint32_t ms)
{
	uint32_t t0 = platform_millis();
	while ((platform_millis() - t0) < ms)
	{
		asm("nop");
	};
}

/* -------------------------------------------------------------------------- */