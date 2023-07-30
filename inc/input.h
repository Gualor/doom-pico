/* Header guard ------------------------------------------------------------- */

#ifndef INPUT_H
#define INPUT_H

/* Includes ----------------------------------------------------------------- */

#include <stdbool.h>

/* Data types --------------------------------------------------------------- */

typedef enum
{
    UP    = 0b00000001,
    DOWN  = 0b00000010,
    LEFT  = 0b00000100,
    RIGHT = 0b00001000,
    FIRE  = 0b00010000,
    JUMP  = 0b00100000,
    HOME  = 0b01000000,
    EXIT  = 0b10000000
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
 * @brief INPUT check if jump button has been pressed.
 * 
 * @return bool button is pressed
 */
bool input_jump(void);

/**
 * @brief INPUT check if home button has been pressed.
 * 
 * @return bool button is pressed
 */
bool input_home(void);

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