/* Header guard ------------------------------------------------------------- */

#ifndef DISPLAY_H
#define DISPLAY_H

/* Includes ----------------------------------------------------------------- */

#include <stdint.h>
#include <stdbool.h>

#include "constants.h"

/* Definitions -------------------------------------------------------------- */

#define OPTIMIZE_DISPLAY
#define ZBUFFER_SIZE (SCREEN_WIDTH / Z_RES_DIVIDER)
#define DISPLAY_BUF_SIZE (SCREEN_WIDTH * ((SCREEN_HEIGHT + 7) / 8))

void display_init();
void display_update(void);
void display_clear(void);
void display_invert(void);
void display_fade(uint8_t intensity, bool color);

// void fps(void);
float getActualFps(void);

bool display_get_gradient(uint8_t x, uint8_t y, uint8_t i);
void display_draw_byte(uint8_t x, uint8_t y, uint8_t b);
uint8_t display_get_byte(uint8_t x, uint8_t y);
void display_draw_pixel(int8_t x, int8_t y, bool color, bool raycasterViewport);
bool display_get_pixel(int16_t x, int16_t y);
void display_draw_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool color);
void display_draw_vline(uint8_t x, int8_t start_y, int8_t end_y, uint8_t intensity);
void display_draw_bitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w,
						 int16_t h, uint16_t color);
void display_draw_sprite(int8_t x, int8_t y, const uint8_t bitmap[],
						 const uint8_t mask[], int16_t w, int16_t h,
						 uint8_t sprite, float distance);
void display_draw_char(int8_t x, int8_t y, char ch);
void display_draw_text(int8_t x, int8_t y, char *txt, uint8_t space);
void display_draw_int(uint8_t x, uint8_t y, uint8_t num);

// FPS control
extern float delta;
extern uint32_t lastFrameTime;

// We don't handle more than MAX_RENDER_DEPTH depth, so we can safety store
// z values in a byte with 1 decimal and save some memory,
extern uint8_t zbuffer[ZBUFFER_SIZE];
extern uint8_t display_buf[DISPLAY_BUF_SIZE];

#endif /* DISPLAY_H */