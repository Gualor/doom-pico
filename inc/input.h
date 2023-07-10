/* Header guard ------------------------------------------------------------- */

#ifndef INPUT_H
#define INPUT_H

/* Includes ----------------------------------------------------------------- */

#include <stdbool.h>

/* Data types --------------------------------------------------------------- */

typedef enum
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
} Buttons;

/* Function prototypes ------------------------------------------------------ */

/**
 * @brief INPUT initialize input state.
 * 
 */
void input_init(void);

/**
 * @brief INPUT execute platform input update.
 * 
 */
void input_update(void);

/**
 * @brief INPUT check if up button has been pressed.
 * 
 * @return bool button is pressed
 */
bool input_up(void);

/**
 * @brief INPUT check if down button has been pressed.
 * 
 * @return bool button is pressed
 */
bool input_down(void);

/**
 * @brief INPUT check if left button has been pressed.
 * 
 * @return bool button is pressed
 */
bool input_left(void);

/**
 * @brief INPUT check if right button has been pressed.
 * 
 * @return bool button is pressed
 */
bool input_right(void);

/**
 * @brief INPUT check if fire button has been pressed.
 * 
 * @return bool button is pressed
 */
bool input_fire(void);

/**
 * @brief INPUT check if exit button has been pressed.
 * 
 * @return bool button is pressed
 */
bool input_exit(void);

/* Global variables --------------------------------------------------------- */

extern volatile uint16_t input_button;

#endif /* INPUT_H */

/* -------------------------------------------------------------------------- */