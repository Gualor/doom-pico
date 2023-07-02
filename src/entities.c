/* Includes ----------------------------------------------------------------- */

#include <stdint.h>

#include "entities.h"
#include "types.h"
#include "constants.h"

/* Function definitions ----------------------------------------------------- */

Player entities_create_player(uint8_t x, uint8_t y)
{
	return (Player){
		.pos = {x + 0.5f, y + 0.5},
		.dir = {1.0f, 0.0f},
		.plane = {0.0f, -0.66f},
		.velocity = 0.0f,
		.health = 100,
		.keys = 0};
}

Entity entities_create_entity(uint8_t type, uint8_t x, uint8_t y, uint8_t state,
							  uint8_t health)
{
	return (Entity){
		.uid = create_uid(type, x, y),
		.pos = {x + 0.5f, y + 0.5f},
		.state = state,
		.health = health,
		.distance = 0,
		.timer = 0};
}

StaticEntity entities_create_static(UID uid, uint8_t x, uint8_t y, bool active)
{
	return (StaticEntity){
		.uid = uid,
		.x = x,
		.y = y,
		.active = active};
}

/* -------------------------------------------------------------------------- */