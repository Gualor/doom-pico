/* Header guard ------------------------------------------------------------- */

#ifndef ENTITIES_H
#define ENTITIES_H

/* Includes ----------------------------------------------------------------- */

#include <stdint.h>
#include <stdbool.h>

#include "types.h"

/* Definitions -------------------------------------------------------------- */

// Shortcuts
#define entities_create_enemy(x, y) \
	entities_create_entity(E_ENEMY, x, y, S_STAND, 100)
#define entities_create_medikit(x, y) \
	entities_create_entity(E_MEDIKIT, x, y, S_STAND, 0)
#define entities_create_key(x, y) \
	entities_create_entity(E_KEY, x, y, S_STAND, 0)
#define entities_create_fireball(x, y, dir) \
	entities_create_entity(E_FIREBALL, x, y, S_STAND, dir)

/* Data types --------------------------------------------------------------- */

typedef enum
{
	S_STAND = 0,
	S_ALERT = 1,
	S_FIRING = 2,
	S_MELEE = 3,
	S_HIT = 4,
	S_DEAD = 5,
	S_HIDDEN = 6,
	S_OPEN = 7,
	S_CLOSE = 8,
} EntityStatus;

typedef struct
{
	Coords pos;
	Coords dir;
	Coords plane;
	float velocity;
	uint8_t health;
	uint8_t keys;
} Player;

typedef struct
{
	UID uid;
	Coords pos;
	uint8_t state;
	uint8_t health; // angle for fireballs
	uint8_t distance;
	uint8_t timer;
} Entity;

typedef struct
{
	UID uid;
	uint8_t x;
	uint8_t y;
	bool active;
} StaticEntity;

/* Function prototypes ------------------------------------------------------ */

Player entities_create_player(uint8_t x, uint8_t y);
Entity entities_create_entity(uint8_t type, uint8_t x, uint8_t y, uint8_t state,
							  uint8_t health);
StaticEntity entities_create_static(UID uid, uint8_t x, uint8_t y, bool active);

#endif /* ENTITIES_H */

/* -------------------------------------------------------------------------- */