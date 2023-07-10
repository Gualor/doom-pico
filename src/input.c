/* Includes ----------------------------------------------------------------- */

#include <stdint.h>

#include "input.h"
#include "constants.h"
#include "platform.h"

/* Global variables --------------------------------------------------------- */

volatile uint16_t input_button;

/* Function definitions ----------------------------------------------------- */

void input_init(void)
{
    input_button = 0;
}

void input_update(void)
{
    input_button = 0;
	platform_input_update();
}

bool input_left(void)
{
    return input_button & LEFT;
};

bool input_right(void)
{
    return input_button & RIGHT;
};

bool input_up(void)
{
    return input_button & UP;
};

bool input_down(void)
{
    return input_button & DOWN;
};

bool input_fire(void)
{
    return input_button & Y;
};

bool input_exit(void)
{
    return input_button & SELECT;
}

/* -------------------------------------------------------------------------- */