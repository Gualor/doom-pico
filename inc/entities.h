/* Header guard ------------------------------------------------------------- */

#ifndef ENTITIES_H
#define ENTITIES_H

/* Includes ----------------------------------------------------------------- */

#include <stdint.h>
#include <stdbool.h>

#include "coords.h"

/* Definitions -------------------------------------------------------------- */

#define UID_NULL 0

/* Data types --------------------------------------------------------------- */

typedef uint16_t EntityUID;

typedef enum
{
    E_FLOOR = 0x0,      // . (also null)
    E_WALL = 0xF,       // #
    E_PLAYER = 0x1,     // P
    E_ENEMY = 0x2,      // E
    E_DOOR = 0x4,       // D
    E_LOCKEDDOOR = 0x5, // L
    E_EXIT = 0x7,       // X
    E_MEDKIT = 0x8,     // M
    E_KEY = 0x9,        // K
    E_FIREBALL = 0xa,   // not in map
} EntityType;

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
    EntityUID uid;
    Coords pos;
    uint8_t state;
    uint8_t health; // angle for fireballs
    uint8_t distance;
    uint8_t timer;
} Entity;

typedef struct
{
    EntityUID uid;
    uint8_t x;
    uint8_t y;
    bool active;
} StaticEntity;

/* Function prototypes ------------------------------------------------------ */

EntityUID entities_get_uid(EntityType type, uint8_t x, uint8_t y);
EntityType entities_get_type(EntityUID uid);

/* Function definitions ----------------------------------------------------- */

static inline Player entities_create_player(uint8_t x, uint8_t y)
{
    return (Player){
        .pos = {x + 0.5f, y + 0.5},
        .dir = {1.0f, 0.0f},
        .plane = {0.0f, -0.66f},
        .velocity = 0.0f,
        .health = 100,
        .keys = 0};
}

static inline Entity entities_create_enemy(uint8_t x, uint8_t y)
{
    return (Entity){
        .uid = entities_get_uid(E_ENEMY, x, y),
        .pos = {x + 0.5f, y + 0.5f},
        .state = S_STAND,
        .health = 100,
        .distance = 0,
        .timer = 0};
}

static inline Entity entities_create_medkit(uint8_t x, uint8_t y)
{
    return (Entity){
        .uid = entities_get_uid(E_MEDKIT, x, y),
        .pos = {x + 0.5f, y + 0.5f},
        .state = S_STAND,
        .health = 100,
        .distance = 0,
        .timer = 0};
}

static inline Entity entities_create_key(uint8_t x, uint8_t y)
{
    return (Entity){
        .uid = entities_get_uid(E_KEY, x, y),
        .pos = {x + 0.5f, y + 0.5f},
        .state = S_STAND,
        .health = 100,
        .distance = 0,
        .timer = 0};
}

static inline Entity entities_create_fireball(uint8_t x, uint8_t y, uint8_t dir)
{
    return (Entity){
        .uid = entities_get_uid(E_KEY, x, y),
        .pos = {x + 0.5f, y + 0.5f},
        .state = S_STAND,
        .health = dir,
        .distance = 0,
        .timer = 0};
}

#endif /* ENTITIES_H */

/* -------------------------------------------------------------------------- */