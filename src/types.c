#include <stdint.h>
#include <math.h>

#include "types.h"
#include "constants.h"
#include "utils.h"

Coords create_coords(float x, float y)
{
	return (Coords){x, y};
}

uint8_t coords_distance(Coords *a, Coords *b)
{
	return sqrtf(POW2(a->x - b->x) + POW2(a->y - b->y)) * DISTANCE_MULTIPLIER;
}

UID create_uid(uint8_t type, uint8_t x, uint8_t y)
{
	return ((y << LEVEL_WIDTH_BASE) | x) << 4 | type;
}

uint8_t uid_get_type(UID uid)
{
	return uid & 0x0F;
}
