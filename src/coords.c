/* Includes ----------------------------------------------------------------- */

#include <stdint.h>
#include <math.h>

#include "coords.h"
#include "constants.h"
#include "utils.h"

/* Function definitions ----------------------------------------------------- */

uint8_t coords_compute_distance(Coords *a, Coords *b)
{
	return sqrtf(POW2(a->x - b->x) + POW2(a->y - b->y)) * DISTANCE_MULTIPLIER;
}

/* -------------------------------------------------------------------------- */