/* Header guard ------------------------------------------------------------- */

#ifndef DISPLAY_H
#define DISPLAY_H

/* Includes ----------------------------------------------------------------- */

#include <stdint.h>
#include <stdbool.h>

#include "ssd1306.h"
#include "constants.h"

/* Definitions -------------------------------------------------------------- */

// This is slightly faster than bitRead (also bits are read from left to right)
const static uint8_t bit_mask[8] = {128, 64, 32, 16, 8, 4, 2, 1};
#define read_bit(b, n) ((bit_mask[n] >> b) & 0x1)

/* Function definitions ----------------------------------------------------- */

void setupDisplay(void);
void fps(void);
bool getGradientPixel(uint8_t x, uint8_t y, uint8_t i);
float getActualFps(void);
void fadeScreen(uint8_t intensity, bool color);
void drawByte(uint8_t x, uint8_t y, uint8_t b);
uint8_t getByte(uint8_t x, uint8_t y);
void drawPixel(int8_t x, int8_t y, bool color, bool raycasterViewport);
void drawVLine(uint8_t x, int8_t start_y, int8_t end_y, uint8_t intensity);
void drawSprite(int8_t x, int8_t y, const uint8_t bitmap[],
				const uint8_t mask[], int16_t w, int16_t h, uint8_t sprite,
				float distance);
void drawChar(int8_t x, int8_t y, char ch);
void drawText(int8_t x, int8_t y, char *txt, bool space);
void drawInteger(uint8_t x, uint8_t y, uint8_t num);

/* Global variables --------------------------------------------------------- */

// Initialize screen. Following line is for OLED 128x64 connected by I2C
// Adafruit_SSD1306<SCREEN_WIDTH, SCREEN_HEIGHT> display;

// FPS control
float delta = 1;
uint32_t lastFrameTime = 0;

#ifdef OPTIMIZE_SSD1306
// Optimizations for SSD1306 handles buffer directly
uint8_t *display_buf;
#endif

// We don't handle more than MAX_RENDER_DEPTH depth, so we can safety store
// z values in a byte with 1 decimal and save some memory,
uint8_t zbuffer[ZBUFFER_SIZE];

#endif /* DISPLAY_H */

/* -------------------------------------------------------------------------- */