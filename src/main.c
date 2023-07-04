#include <stdint.h>

#include "raylib.h"
#include "game.h"

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

void main(void)
{
	GameConfig game_cfg = {
		.draw_start = platform_draw_start,
		.draw_stop = platform_draw_stop,
		.draw_pixel = platform_draw_pixel
	};

	game_run(&game_cfg);
}
