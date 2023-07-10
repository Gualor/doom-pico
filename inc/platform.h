/* Header guard ------------------------------------------------------------- */

#ifndef PLATFORM_H
#define PLATFORM_H

/* Includes ----------------------------------------------------------------- */

#include <stdint.h>
#include <stdbool.h>

/* Functions prototypes ----------------------------------------------------- */

/**
 * @brief PLATFORM initialize user-defined functions.
 * 
 */
void platform_init(void);

/**
 * @brief PLATFORM start drawing a new frame.
 * 
 */
void platform_draw_start(void);

/**
 * @brief PLATFORM stop drawing current frame.
 * 
 */
void platform_draw_stop(void);

/**
 * @brief PLATFORM write pixel value to screen.
 * 
 * @param x     X coordinate
 * @param y     Y coordinate
 * @param color Pixel color
 */
void platform_draw_pixel(uint8_t x, uint8_t y, bool color);

/**
 * @brief PLATFORM play audio effect through speaker.
 * 
 */
void platform_audio_play(void);

/**
 * @brief PLATFORM read user controls and update button state.
 * 
 */
void platform_input_update(void);

/**
 * @brief PLATFORM get time in milliseconds from start of execution.
 * 
 * @return uint32_t Start time in milliseconds
 */
uint32_t platform_millis(void);

/**
 * @brief PLATFORM apply blocking delay in milliseconds.
 * 
 * @param ms Delay in milliseconds
 */
void platform_delay(uint32_t ms);

#endif /* PLATFORM_H */

/* -------------------------------------------------------------------------- */