/* Includes ----------------------------------------------------------------- */

#include <stdint.h>

#include "input.h"
#include "constants.h"
#include "platform.h"

/* Global variables --------------------------------------------------------- */

volatile uint16_t input_button;

/* Function definitions ----------------------------------------------------- */

/**
 * @brief INPUT initialize input state.
 * 
 */
void input_init(void)
{
    input_button = 0;
}

/**
 * @brief INPUT execute platform input update.
 * 
 */
void input_update(void)
{
    input_button = 0;
	platform_input_update();
}

/**
 * @brief INPUT check if up button has been pressed.
 * 
 * @return bool button is pressed
 */
bool input_up(void)
{
    return input_button & UP;
};

/**
 * @brief INPUT check if down button has been pressed.
 * 
 * @return bool button is pressed
 */
bool input_down(void)
{
    return input_button & DOWN;
};

/**
 * @brief INPUT check if left button has been pressed.
 * 
 * @return bool button is pressed
 */
bool input_left(void)
{
    return input_button & LEFT;
};

/**
 * @brief INPUT check if right button has been pressed.
 * 
 * @return bool button is pressed
 */
bool input_right(void)
{
    return input_button & RIGHT;
};

/**
 * @brief INPUT check if fire button has been pressed.
 * 
 * @return bool button is pressed
 */
bool input_fire(void)
{
    return input_button & FIRE;
};

/**
 * @brief INPUT check if jump button has been pressed.
 * 
 * @return bool button is pressed
 */
bool input_jump(void)
{
    return input_button & JUMP;
};

/**
 * @brief INPUT check if home button has been pressed.
 * 
 * @return bool button is pressed
 */
bool input_home(void)
{
    return input_button & HOME;
};

/**
 * @brief INPUT check if exit button has been pressed.
 * 
 * @return bool button is pressed
 */
bool input_exit(void)
{
    return input_button & EXIT;
}

/* -------------------------------------------------------------------------- */