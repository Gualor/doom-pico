/* Includes ----------------------------------------------------------------- */

#include <stdint.h>

#include "utils.h"
#include "platform.h"

/* Function definitions ----------------------------------------------------- */

/**
 * @brief UTILS get time in milliseconds from start of execution.
 * 
 * @return uint32_t Start time in milliseconds
 */
uint32_t millis(void)
{
    return platform_millis();
}

/**
 * @brief UTILS apply blocking delay in milliseconds.
 * 
 * @param ms Delay in milliseconds
 */
void delay(uint32_t ms)
{
    platform_delay(ms);
}

/* -------------------------------------------------------------------------- */