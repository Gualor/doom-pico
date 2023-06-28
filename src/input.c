/* Includes ----------------------------------------------------------------- */

#include <stdbool.h>

#include "input.h"
#include "constants.h"

/* Definitions -------------------------------------------------------------- */

#ifdef USE_INPUT_PULLUP
#define INPUT_MODE INPUT_PULLUP
#define INPUT_STATE LOW
#else
#define INPUT_MODE INPUT
#define INPUT_STATE HIGH
#endif

#ifdef SNES_CONTROLLER

/* Global variables --------------------------------------------------------- */

uint16_t buttons = 0;

/* Function definitions ----------------------------------------------------- */

void input_setup(void)
{
	// // Set DATA_CLOCK normally HIGH
	// pinMode(DATA_CLOCK, OUTPUT);
	// digitalWrite(DATA_CLOCK, HIGH);

	// // Set DATA_LATCH normally LOW
	// pinMode(DATA_LATCH, OUTPUT);
	// digitalWrite(DATA_LATCH, LOW);

	// // Set DATA_SERIAL normally HIGH
	// pinMode(DATA_SERIAL, OUTPUT);
	// digitalWrite(DATA_SERIAL, HIGH);
	// pinMode(DATA_SERIAL, INPUT);
}

void getControllerData(void)
{
	// // Latch for 12us
	// digitalWrite(DATA_LATCH, HIGH);
	// delayMicroseconds(12);
	// digitalWrite(DATA_LATCH, LOW);
	// delayMicroseconds(6);
	// buttons = 0;
	// // Retrieve button presses from shift register by pulling the clock high for 6us
	// for (uint8_t i = 0; i < 16; ++i) {
	// 	digitalWrite(DATA_CLOCK, LOW);
	// 	delayMicroseconds(6);
	// 	buttons |= !digitalRead(DATA_SERIAL) << i;
	// 	digitalWrite(DATA_CLOCK, HIGH);
	// 	delayMicroseconds(6);
	// }
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

bool input_start(void)
{
	return buttons & START;
}

#else

void input_setup(void)
{
	// pinMode(K_LEFT, INPUT_MODE);
	// pinMode(K_RIGHT, INPUT_MODE);
	// pinMode(K_UP, INPUT_MODE);
	// pinMode(K_DOWN, INPUT_MODE);
	// pinMode(K_FIRE, INPUT_MODE);
}

bool input_left(void)
{
	// return digitalRead(K_LEFT) == INPUT_STATE;
	return false;
};

bool input_right(void)
{
	// return digitalRead(K_RIGHT) == INPUT_STATE;
	return false;
};

bool input_up(void)
{
	// return digitalRead(K_UP) == INPUT_STATE;
	return false;
};

bool input_down(void)
{
	// return digitalRead(K_DOWN) == INPUT_STATE;
	return false;
};

bool input_fire(void)
{
	// return digitalRead(K_FIRE) == INPUT_STATE;
	return false;
};

#endif

/* -------------------------------------------------------------------------- */