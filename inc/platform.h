#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

uint32_t millis(void);
void delay(uint32_t ms);
void playSound(const uint8_t *buf, uint8_t len);
void sound_init(void);

#endif