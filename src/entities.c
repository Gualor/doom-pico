/* Includes ----------------------------------------------------------------- */

#include <stdint.h>

#include "entities.h"
#include "constants.h"

/* Function definitions ----------------------------------------------------- */

/**
 * @brief ENTITIES get UID from entity type and location.
 * 
 * @param type Entity type
 * @param x    X coordinate
 * @param y    Y coordinate
 * @return EntityUID Entity UID number
 */
EntityUID entities_get_uid(EntityType type, uint8_t x, uint8_t y)
{
    return ((y << LEVEL_WIDTH_BASE) | x) << 4 | type;
}

/**
 * @brief ENTITIES get entity type from UID.
 * 
 * @param uid Entity UID
 * @return EntityType Entity type
 */
EntityType entities_get_type(EntityUID uid)
{
    return uid & 0x0f;
}

/* -------------------------------------------------------------------------- */