/* Includes ----------------------------------------------------------------- */

#include <stdint.h>

#include "entities.h"
#include "types.h"
#include "constants.h"

/* Function definitions ----------------------------------------------------- */

struct Entity create_entity(uint8_t type, uint8_t x, uint8_t y,
							uint8_t initialState, uint8_t initialHealth)
{
	UID uid = create_uid(type, x, y);
	struct Coords pos = create_coords((double)x + .5, (double)y + .5);
	struct Entity new_entity = {uid, pos, initialState, initialHealth, 0, 0};
	return new_entity;
}

struct StaticEntity create_static_entity(UID uid, uint8_t x, uint8_t y,
										 bool active)
{
	return (struct StaticEntity){uid, x, y, active};
}

/* -------------------------------------------------------------------------- */