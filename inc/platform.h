#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

uint32_t millis(void);
void delay(uint32_t ms);

extern volatile uint32_t init_clock;

#endif