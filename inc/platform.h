/* Header guard ------------------------------------------------------------- */

#ifndef PLATFORM_H
#define PLATFORM_H

/* Includes ----------------------------------------------------------------- */

#include <stdint.h>
#include <stdbool.h>

/* Functions prototypes ----------------------------------------------------- */

void platform_init(void);
void platform_draw_start(void);
void platform_draw_stop(void);
void platform_draw_pixel(uint8_t x, uint8_t y, bool color);
void platform_play_audio(bool play);
uint32_t platform_millis(void);
void platform_delay(uint32_t ms);

#endif /* PLATFORM_H */

/* -------------------------------------------------------------------------- */