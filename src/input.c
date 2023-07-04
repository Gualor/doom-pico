/* Includes ----------------------------------------------------------------- */

#include <stdint.h>

#include "input.h"
#include "constants.h"

/** TODO: Remove raylib dependencies from here */
#include "raylib.h"

/* Global variables --------------------------------------------------------- */

static volatile uint16_t buttons;

/* Function definitions ----------------------------------------------------- */

void input_init(void)
{
	buttons = 0;
}

void input_update(void)
{
	buttons = 0;

	if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
		buttons = UP;
	else if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
		buttons = DOWN;
	else if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
		buttons = LEFT;
	else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
		buttons = RIGHT;
	else if (IsKeyDown(KEY_SPACE))
		buttons = Y;
	else if (IsKeyDown(KEY_ESCAPE))
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

bool input_exit(void)
{
	return buttons & SELECT;
}

/* -------------------------------------------------------------------------- */