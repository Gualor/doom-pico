/* Header guard ------------------------------------------------------------- */

#ifndef DISPLAY_H
#define DISPLAY_H

/* Includes ----------------------------------------------------------------- */

#include <stdint.h>
#include <stdbool.h>

#include "constants.h"

/* Definitions -------------------------------------------------------------- */

#define DISPLAY_SIZE (SCREEN_WIDTH * ((SCREEN_HEIGHT + 7) / 8))

/* Function definitions ----------------------------------------------------- */

void setupDisplay(void);
void updateDisplay(void);
void clearDisplay(void);
void invertDisplay(bool i);
void clearRect(uint8_t x, uint8_t y, uint8_t h, uint8_t w);

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
void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w,
				int16_t h, bool color);
bool getPixel(int16_t x, int16_t y);

/* Global variables --------------------------------------------------------- */

// FPS control
extern float delta;
extern uint32_t lastFrameTime;

// We don't handle more than MAX_RENDER_DEPTH depth, so we can safety store
// z values in a byte with 1 decimal and save some memory,
extern uint8_t display[DISPLAY_SIZE];
extern uint8_t zbuffer[ZBUFFER_SIZE];

#endif /* DISPLAY_H */

/* -------------------------------------------------------------------------- */