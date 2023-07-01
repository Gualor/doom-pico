#include <stdio.h>
#include <string.h>

#include "display.h"
#include "sprites.h"
#include "platform.h"
#include "utils.h"

// Temporary import, no platform dependent functions here
#include "raylib.h"

float delta = 1;
uint32_t lastFrameTime = 0;
uint8_t zbuffer[ZBUFFER_SIZE];
uint8_t display_buf[DISPLAY_BUF_SIZE];

void display_init(void)
{
	delta = 1;
	lastFrameTime = 0;

	// initialize z buffer
	memset(display_buf, 0x00, DISPLAY_BUF_SIZE);
	memset(zbuffer, 0xff, ZBUFFER_SIZE);
}

void display_update(void)
{
	for (uint8_t x = 0; x < SCREEN_WIDTH; x++)
	{
		for (uint8_t y = 0; y < SCREEN_HEIGHT; y++)
		{
			uint8_t color = display_get_pixel(x, y);
			DrawPixel(x, y, color ? WHITE : BLACK);
		}
	}
}

void display_clear(void)
{
	memset(display_buf, 0x00, DISPLAY_BUF_SIZE);
}

void display_invert(void)
{
	for (uint16_t i = 0; i < DISPLAY_BUF_SIZE; i++)
		display_buf[i] = ~display_buf[i];
}

void display_fade(uint8_t intensity, bool color)
{
	for (uint8_t x = 0; x < SCREEN_WIDTH; x++)
	{
		for (uint8_t y = 0; y < SCREEN_HEIGHT; y++)
		{
			if (display_get_gradient(x, y, intensity))
				display_draw_pixel(x, y, color, false);
		}
	}
}

// Adds a delay to limit play to specified fps
// Calculates also delta to keep movement consistent in lower framerates
// void fps(void)
// {
// 	while (millis() - lastFrameTime < FRAME_TIME)
// 	{
// 	};
// 	delta = (float)(millis() - lastFrameTime) / FRAME_TIME;
// 	lastFrameTime = millis();
// }

float getActualFps(void)
{
	return 1000 / (FRAME_TIME * delta);
}

// Faster way to render vertical bits
void display_draw_byte(uint8_t x, uint8_t y, uint8_t b)
{
	display_buf[(y / 8) * SCREEN_WIDTH + x] = b;
}

uint8_t display_get_byte(uint8_t x, uint8_t y)
{
	return display_buf[(y / 8) * SCREEN_WIDTH + x];
}

bool display_get_gradient(uint8_t x, uint8_t y, uint8_t i)
{
	if (i == 0)
		return 0;
	if (i >= GRADIENT_COUNT - 1)
		return 1;

	uint8_t index = MAX(0, MIN(GRADIENT_COUNT - 1, i)) *
						GRADIENT_WIDTH * GRADIENT_HEIGHT +					  // gradient index
					y * GRADIENT_WIDTH % (GRADIENT_WIDTH * GRADIENT_HEIGHT) + // y byte offset
					x / GRADIENT_HEIGHT % GRADIENT_WIDTH;					  // x byte offset

	// return the bit based on x
	return READ_BIT(*(gradient + index), x % 8);
}

// Faster drawPixel than display.display_draw_pixel.
// Avoids some checks to make it faster.
void display_draw_pixel(int8_t x, int8_t y, bool color, bool raycasterViewport)
{
	// prevent write out of screen buffer
	if ((x < 0) || (x >= SCREEN_WIDTH) || (y < 0) ||
		(y >= (raycasterViewport ? RENDER_HEIGHT : SCREEN_HEIGHT)))
		return;

	if (color)
		display_buf[x + (y / 8) * SCREEN_WIDTH] |= (1 << (y & 7)); // White
	else
		display_buf[x + (y / 8) * SCREEN_WIDTH] &= ~(1 << (y & 7)); // Black
}

bool display_get_pixel(int16_t x, int16_t y)
{
	if ((x >= 0) && (x < SCREEN_WIDTH) && (y >= 0) && (y < SCREEN_HEIGHT))
		return (display_buf[x + (y / 8) * SCREEN_WIDTH] & (1 << (y & 7)));

	return false; // Pixel out of bounds
}

void display_draw_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool color)
{
	for (uint8_t i = x; i < w; i++)
	{
		for (uint8_t j = y; j < h; j++)
			display_draw_pixel(i, j, color, false);
	}
}

// For raycaster only
// Custom draw Vertical lines that fills with a pattern to simulate
// different brightness. Affected by RES_DIVIDER
void display_draw_vline(uint8_t x, int8_t start_y, int8_t end_y, uint8_t intensity)
{
	int8_t y;
	int8_t lower_y = MAX(MIN(start_y, end_y), 0);
	int8_t higher_y = MIN(MAX(start_y, end_y), RENDER_HEIGHT - 1);
	uint8_t c;

#ifdef OPTIMIZE_DISPLAY
	uint8_t bp;
	uint8_t b;
	for (c = 0; c < RES_DIVIDER; c++)
	{
		y = lower_y;
		b = 0;
		while (y <= higher_y)
		{
			bp = y % 8;
			b = b | display_get_gradient(x + c, y, intensity) << bp;

			if (bp == 7)
			{
				// write the whole byte
				display_draw_byte(x + c, y, b);
				b = 0;
			}

			y++;
		}

		// draw last byte
		if (bp != 7)
		{
			display_draw_byte(x + c, y - 1, b);
		}
	}
#else
	y = lower_y;
	while (y <= higher_y)
	{
		for (c = 0; c < RES_DIVIDER; c++)
		{
			// bypass black pixels
			if (display_get_gradient(x + c, y, intensity))
			{
				display_draw_pixel(x + c, y, 1, true);
			}
		}

		y++;
	}
#endif
}

void display_draw_bitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w,
						 int16_t h, uint16_t color)
{

	int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
	uint8_t byte = 0;

	for (int16_t j = 0; j < h; j++, y++)
	{
		for (int16_t i = 0; i < w; i++)
		{
			if (i & 7)
				byte <<= 1;
			else
				byte = bitmap[j * byteWidth + i / 8];

			if (byte & 0x80)
				display_draw_pixel(x + i, y, color, true);
		}
	}
}

// Custom display_draw_bitmap method with scale support, mask, zindex and pattern filling
void display_draw_sprite(int8_t x, int8_t y, const uint8_t bitmap[],
						 const uint8_t mask[], int16_t w, int16_t h,
						 uint8_t sprite, float distance)
{
	uint8_t tw = (float)w / distance;
	uint8_t th = (float)h / distance;
	uint8_t byte_width = w / 8;
	uint8_t pixel_size = MAX(1, 1.0 / distance);
	uint16_t sprite_offset = byte_width * h * sprite;

	bool pixel;
	bool maskPixel;

	// Don't draw the whole sprite if the anchor is hidden by z buffer
	// Not checked per pixel for performance reasons
	if (zbuffer[MIN(MAX(x, 0), ZBUFFER_SIZE - 1) / Z_RES_DIVIDER] <
		distance * DISTANCE_MULTIPLIER)
		return;

	for (uint8_t ty = 0; ty < th; ty += pixel_size)
	{
		// Don't draw out of screen
		if ((y + ty < 0) || (y + ty >= RENDER_HEIGHT))
			continue;

		uint8_t sy = ty * distance; // The y from the sprite

		for (uint8_t tx = 0; tx < tw; tx += pixel_size)
		{
			uint8_t sx = tx * distance; // The x from the sprite
			uint16_t byte_offset = sprite_offset + sy * byte_width + sx / 8;

			// Don't draw out of screen
			if (x + tx < 0 || x + tx >= SCREEN_WIDTH)
				continue;

			maskPixel = READ_BIT(*(mask + byte_offset), sx % 8);

			if (maskPixel)
			{
				pixel = READ_BIT(*(bitmap + byte_offset), sx % 8);
				for (uint8_t ox = 0; ox < pixel_size; ox++)
				{
					for (uint8_t oy = 0; oy < pixel_size; oy++)
					{
						display_draw_pixel(x + tx + ox, y + ty + oy, pixel, true);
					}
				}
			}
		}
	}
}

// Draw a single character.
// Made for a custom font with some useful sprites. Char size 4 x 6
// Uses less memory than display.print()
void display_draw_char(int8_t x, int8_t y, char ch)
{
	uint8_t c = 0;
	uint8_t n;
	uint8_t bOffset;
	uint8_t b;
	uint8_t line = 0;

	// Find the character
	while (CHAR_MAP[c] != ch && CHAR_MAP[c] != '\0')
		c++;

	bOffset = c / 2;
	for (; line < CHAR_HEIGHT; line++)
	{
		b = *(bmp_font + (line * bmp_font_width + bOffset));
		for (n = 0; n < CHAR_WIDTH; n++)
			if (READ_BIT(b, (c % 2 == 0 ? 0 : 4) + n))
				display_draw_pixel(x + n, y + line, 1, false);
	}
}

void display_draw_text(int8_t x, int8_t y, char *txt, uint8_t space)
{
	uint8_t pos = x;
	uint8_t i = 0;
	while ((pos < SCREEN_WIDTH) && (txt[i] != '\0'))
	{
		display_draw_char(pos, y, txt[i]);
		pos += CHAR_WIDTH + space;
		i++;
	}
}

// Draw an integer (3 digit max!)
void display_draw_int(uint8_t x, uint8_t y, uint8_t num)
{
	char buf[4]; // 3 char + \0
	sprintf(buf, "%d", num);
	display_draw_text(x, y, buf, true);
}