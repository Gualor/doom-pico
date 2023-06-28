/* Header guard ------------------------------------------------------------- */

#ifndef INPUT_H
#define INPUT_H

/* Includes ----------------------------------------------------------------- */

#include <stdbool.h>

/* Data types --------------------------------------------------------------- */

enum BUTTONS
{
	B = 0x0001,
	Y = 0x0002,
	SELECT = 0x0004,
	START = 0x0008,
	UP = 0x0010,
	DOWN = 0x0020,
	LEFT = 0x0040,
	RIGHT = 0x0080,
	A = 0x0100,
	X = 0x0200,
	LB = 0x0400,
	RB = 0x0800
};

/* Function definitions ----------------------------------------------------- */

void input_setup(void);
bool input_up(void);
bool input_down(void);
bool input_left(void);
bool input_right(void);
bool input_fire(void);

#ifdef SNES_CONTROLLER
bool input_start(void);
void getControllerData(void);
#endif

#endif /* INPUT_H */

/* -------------------------------------------------------------------------- */