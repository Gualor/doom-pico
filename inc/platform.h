/* Header guard ------------------------------------------------------------- */

#ifndef PLATFORM_H
#define PLATFORM_H

/* Includes ----------------------------------------------------------------- */

#include <stdint.h>
#include <stdbool.h>

/* Functions prototypes ----------------------------------------------------- */

/* Platform screen */
void platform_screen_init(void);
void platform_screen_draw_start(void);
void platform_screen_draw_stop(void);
void platform_screen_draw_pixel(uint8_t x, uint8_t y, bool color);

/* Platform audio */
void platform_audio_init(void);
void platform_audio_play_start(void);
void platform_audio_play_stop(void);

/* Platform utilities */
void platform_utils_init(void);
uint32_t platform_utils_millis(void);
void platform_utils_delay(uint32_t ms);

#endif /* PLATFORM_H */

/* -------------------------------------------------------------------------- */