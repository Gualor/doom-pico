/* Includes ----------------------------------------------------------------- */

#include <stdint.h>

#include "input.h"
#include "constants.h"

/** TODO: Remove raylib dependencies from here */
#include "raylib.h"

/* Global variables --------------------------------------------------------- */

static volatile uint16_t buttons;

/* Function definitions ----------------------------------------------------- */

void input_setup(void)
{
	buttons = 0;
}

void input_update(void)
{
	buttons = 0;

	if (IsKeyDown(KEY_UP))
		buttons = UP;
	else if (IsKeyDown(KEY_DOWN))
		buttons = DOWN;
	else if (IsKeyDown(KEY_LEFT))
		buttons = LEFT;
	else if (IsKeyDown(KEY_RIGHT))
		buttons = RIGHT;
	else if (IsKeyDown(KEY_SPACE))
		buttons = Y;
	else if (IsKeyDown(KEY_BACKSPACE))
		buttons = SELECT;
}

bool input_left(void)
{
	return buttons & LEFT;
};

bool input_right(void)
{
	return buttons & RIGHT;
};

bool input_up(void)
{
	return buttons & UP;
};

bool input_down(void)
{
	return buttons & DOWN;
};

bool input_fire(void)
{
	return buttons & Y;
};

bool input_select(void)
{
	return buttons & SELECT;
}

/* -------------------------------------------------------------------------- */