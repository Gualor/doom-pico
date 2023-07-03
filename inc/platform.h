/* Header guard ------------------------------------------------------------- */

#ifndef PLATFORM_H
#define PLATFORM_H

/* Includes ----------------------------------------------------------------- */

#include <stdint.h>

/* Functions prototypes ----------------------------------------------------- */

uint32_t millis(void);
void delay(uint32_t ms);

/* Global variables --------------------------------------------------------- */

extern volatile uint32_t init_clock;

#endif /* PLATFORM_H */

/* -------------------------------------------------------------------------- */