/* Includes ----------------------------------------------------------------- */

#include <stdint.h>

#include "entities.h"
#include "constants.h"

/* Function definitions ----------------------------------------------------- */

UID entities_get_uid(EType type, uint8_t x, uint8_t y)
{
	return ((y << LEVEL_WIDTH_BASE) | x) << 4 | type;
}

EType entities_get_type(UID uid)
{
	return uid & 0x0f;
}

/* -------------------------------------------------------------------------- */