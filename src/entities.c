/* Includes ----------------------------------------------------------------- */

#include <stdint.h>

#include "entities.h"
#include "constants.h"

/* Function definitions ----------------------------------------------------- */

EntityUID entities_get_uid(EntityType type, uint8_t x, uint8_t y)
{
    return ((y << LEVEL_WIDTH_BASE) | x) << 4 | type;
}

EntityType entities_get_type(EntityUID uid)
{
    return uid & 0x0f;
}

/* -------------------------------------------------------------------------- */