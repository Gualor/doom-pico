/* Includes ----------------------------------------------------------------- */

#include <stdint.h>
#include <math.h>

#include "types.h"
#include "constants.h"

/* Function definitions ----------------------------------------------------- */

float sq(float value)
{
	return value * value;
}

struct Coords create_coords(float x, float y)
{
	return (struct Coords){x, y};
}

uint8_t coords_distance(struct Coords *a, struct Coords *b)
{
	return sqrt(sq(a->x - b->x) + sq(a->y - b->y)) * DISTANCE_MULTIPLIER;
}

UID create_uid(EType type, uint8_t x, uint8_t y)
{
	return (((y << LEVEL_WIDTH_BASE) | x) << 4) | type;
}

uint8_t uid_get_type(UID uid)
{
	return uid & 0x0F;
}

/* -------------------------------------------------------------------------- */