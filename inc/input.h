#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

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

void input_setup(void);
void input_update(void);
bool input_up(void);
bool input_down(void);
bool input_left(void);
bool input_right(void);
bool input_fire(void);
bool input_select(void);

#endif /* INPUT_H */