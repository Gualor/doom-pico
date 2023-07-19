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
    E_FLOOR = 0x0, // . (also null)
    E_WALL = 0xF,  // #
    E_DOOR = 0xD,
    E_DOOR2 = 0xA,
    E_DOOR3 = 0xB,
    E_COLL = 0xC,
    E_PLAYER = 0x1,  // P
    E_ENEMY = 0x2,   // E
    E_EXIT = 0x7,    // X
    E_MEDKIT = 0x8,  // M
    E_KEY = 0x9,     // K
    E_FIREBALL = 0xA // not in map
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
    uint8_t secret;
    uint8_t secret2;
    uint8_t secret3;
    int16_t score;
    bool cheats;
} Player;

typedef struct
{
    EntityUID uid;
    Coords pos;
    uint8_t state;
    uint8_t health;
    uint8_t distance;
    uint8_t timer;
    bool a;
    bool b;
} Entity;

typedef struct
{
    EntityUID uid;
    uint8_t x;
    uint8_t y;
    bool active;
} StaticEntity;

/* Function prototypes ------------------------------------------------------ */

/**
 * @brief ENTITIES get UID from entity type and location.
 *
 * @param type Entity type
 * @param x    X coordinate
 * @param y    Y coordinate
 * @return EntityUID Entity UID number
 */
EntityUID entities_get_uid(EntityType type, uint8_t x, uint8_t y);

/**
 * @brief ENTITIES get entity type from UID.
 *
 * @param uid Entity UID
 * @return EntityType Entity type
 */
EntityType entities_get_type(EntityUID uid);

/* Function definitions ----------------------------------------------------- */

/**
 * @brief ENTITIES create player entity struct.
 *
 * @param x X coordinate
 * @param y Y coordinate
 * @return Player Player entity instance
 */
static inline Player entities_create_player(uint8_t x, uint8_t y)
{
    return (Player){
        .pos = {x + 0.5f, y + 0.5},
        .dir = {1.0f, 0.0f},
        .plane = {0.0f, -0.66f},
        .velocity = 0.0f,
        .health = 100,
        .keys = 10,
        .secret = 0,
        .secret2 = 0,
        .secret3 = 0,
        .score = 0,
        .cheats = false};
}

/**
 * @brief ENTITIES create enemy entity struct.
 *
 * @param x X coordinate
 * @param y Y coordinate
 * @return Entity Enemy entity instance
 */
static inline Entity entities_create_enemy(uint8_t x, uint8_t y)
{
    return (Entity){
        .uid = entities_get_uid(E_ENEMY, x, y),
        .pos = {x + 0.5f, y + 0.5f},
        .state = S_STAND,
        .health = 100,
        .distance = 0,
        .timer = 0,
        .a = false,
        .b = false};
}

/**
 * @brief ENTITIES create medkit entity struct.
 *
 * @param x X coordinate
 * @param y Y coordinate
 * @return Entity Medkit entity instance
 */
static inline Entity entities_create_medkit(uint8_t x, uint8_t y)
{
    return (Entity){
        .uid = entities_get_uid(E_MEDKIT, x, y),
        .pos = {x + 0.5f, y + 0.5f},
        .state = S_STAND,
        .health = 100,
        .distance = 0,
        .timer = 0,
        .a = false,
        .b = false};
}

/**
 * @brief ENTITIES create key entity struct.
 *
 * @param x X coordinate
 * @param y Y coordinate
 * @return Entity Key entity instance
 */
static inline Entity entities_create_key(uint8_t x, uint8_t y)
{
    return (Entity){
        .uid = entities_get_uid(E_KEY, x, y),
        .pos = {x + 0.5f, y + 0.5f},
        .state = S_STAND,
        .health = 100,
        .distance = 0,
        .timer = 0,
        .a = false,
        .b = false};
}

/**
 * @brief ENTITIES create fireball entity struct.
 *
 * @param x   X coordinate
 * @param y   Y coordinate
 * @param dir Angle
 * @return Entity Fireball entity instance
 */
static inline Entity entities_create_fireball(uint8_t x, uint8_t y, uint8_t dir)
{
    return (Entity){
        .uid = entities_get_uid(E_KEY, x, y),
        .pos = {x + 0.5f, y + 0.5f},
        .state = S_STAND,
        .health = dir,
        .distance = 0,
        .timer = 0,
        .a = false,
        .b = false};
}

#endif /* ENTITIES_H */

/* -------------------------------------------------------------------------- */