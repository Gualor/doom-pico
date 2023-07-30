/* Header guard ------------------------------------------------------------- */

#ifndef DISPLAY_H
#define DISPLAY_H

/* Includes ----------------------------------------------------------------- */

#include <stdint.h>
#include <stdbool.h>

#include "constants.h"

/* Definitions -------------------------------------------------------------- */

#define DISPLAY_BUF_SIZE (SCREEN_WIDTH * ((SCREEN_HEIGHT + 7) / 8))

/* Function prototypes ------------------------------------------------------ */

/**
 * @brief DISPLAY initialize display state.
 *
 */
void display_init(void);

/**
 * @brief DISPLAY update screen with display buffer data.
 *
 */
void display_update(void);

/**
 * @brief DISPLAY clear display buffer.
 *
 */
void display_clear(void);

/**
 * @brief DISPLAY invert display buffer.
 *
 */
void display_invert(void);

/**
 * @brief DISPLAY apply fading effect to display buffer.
 *
 * @param intensity Brightness
 * @param color     Pixel color
 */
void display_fade(uint8_t intensity, bool color);

/**
 * @brief DISPLAY get pixel gradient intensity pattern value.
 *
 * @param x X coordinate
 * @param y Y coordinate
 * @param i Intensity
 * @return bool Pixel color
 */
bool display_get_gradient(uint8_t x, uint8_t y, uint8_t i);

/**
 * @brief DISPLAY add delay between frames for stable FPS.
 *
 */
void display_delay_fps(void);

/**
 * @brief DISPLAY get current FPS value.
 *
 * @return float FPS value
 */
float display_get_fps(void);

/**
 * @brief DISPLAY clear screen and start drawing.
 *
 */
void display_draw_start(void);

/**
 * @brief DISPLAY update screen, stop drowing, and add FPS delay.
 *
 */
void display_draw_stop(void);

/**
 * @brief DISPLAY draw pixel to display buffer.
 *
 * @param x       X coordinate
 * @param y       Y coordinate
 * @param color   Pixel color
 * @param raycast Check raycast rendering
 */
void display_draw_pixel(int16_t x, int16_t y, bool color, bool raycast);

/**
 * @brief DISPLAY get pixel value from display buffer.
 *
 * @param x X coordinate
 * @param y Y coordinate
 * @return bool Pixel color
 */
bool display_get_pixel(int16_t x, int16_t y);

/**
 * @brief DISPLAY draw entire byte to display buffer for faster vertical line
 * rendering.
 *
 * NOTE: One byte encodes 8 pixels in a vertical line.
 *
 * @param x    X coordinate
 * @param y    Y coordinate
 * @param byte Display buffer byte
 */
void display_draw_byte(uint8_t x, uint8_t y, uint8_t byte);

/**
 * @brief DISPLAY get full display buffer byte.
 *
 * NOTE: One byte encodes 8 pixels in a vertical line.
 *
 * @param x X coordinate
 * @param y Y coordinate
 * @return uint8_t Display buffer raw byte
 */
uint8_t display_get_byte(uint8_t x, uint8_t y);

/**
 * @brief DISPLAY draw rectangle shape to display buffer.
 *
 * @param x     X coordinate
 * @param y     Y coordinate
 * @param w     Width
 * @param h     Height
 * @param color Pixel color
 */
void display_draw_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool color);

/**
 * @brief DISPLAY draw vertical lines to display buffer.
 * @details Custom draw Vertical lines that fills with a pattern to simulate
 * different brightness.
 *
 * NOTE: For raycasting only, Affected by RES_DIVIDER.
 *
 * @param x       X coordinate
 * @param start_y Y start coordinate
 * @param end_y   Y end coordinate
 * @param i       Intensity
 */
void display_draw_vline(uint8_t x, int8_t start_y, int8_t end_y, uint8_t i);

/**
 * @brief DISPLAY draw bitmap to display buffer.
 *
 * @param x      X coordinate
 * @param y      Y coordinate
 * @param bitmap Bitmap byte array
 * @param w      Width
 * @param h      Height
 * @param color  Color value
 */
void display_draw_bitmap(int16_t x, int16_t y, const uint8_t bitmap[],
                         int16_t w, int16_t h, bool color);

/**
 * @brief DISPLAY draw sprite to display buffer.
 * @details Custom display_draw_bitmap method with scale support, mask, zindex
 * and pattern filling
 *
 * @param x        X coordinate
 * @param y        Y coordinate
 * @param bitmap   Bitmap byte array
 * @param mask     Bitmap mask byte array
 * @param w        Width
 * @param h        Height
 * @param sprite   Sprite type
 * @param distance Distance from camera
 */
void display_draw_sprite(int8_t x, int8_t y, const uint8_t bitmap[],
                         const uint8_t mask[], int16_t w, int16_t h,
                         uint8_t sprite, float distance);

/**
 * @brief DISPLAY draw single character to display buffer.
 * @details Made for a custom font with some useful sprites. Char size 4 x 6.
 *
 * @param x  X coordinate
 * @param y  Y coordinate
 * @param ch ASCII character
 */
void display_draw_char(int8_t x, int8_t y, char ch);

/**
 * @brief DISPLAY draw string to display buffer.
 *
 * @param x     X coordinate
 * @param y     Y coordinate
 * @param txt   ASCII string
 * @param space Spacing between letters
 */
void display_draw_text(int8_t x, int8_t y, char *txt, uint8_t space);

/**
 * @brief DISPLAY draw an integer number to display buffer.
 *
 * NOTE: Up to 3 digit max.
 *
 * @param x   X coordinate
 * @param y   Y coordinate
 * @param num Integer number
 */
void display_draw_int(uint8_t x, uint8_t y, uint8_t num);

/* Global variables --------------------------------------------------------- */

extern uint8_t zbuffer[ZBUFFER_SIZE];
extern uint8_t display_buf[DISPLAY_BUF_SIZE];
extern float delta_time;

#endif /* DISPLAY_H */

/* -------------------------------------------------------------------------- */