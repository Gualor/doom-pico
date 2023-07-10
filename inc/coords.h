/* Header guard ------------------------------------------------------------- */

#ifndef COORDS_H
#define COORDS_H

/* Includes ----------------------------------------------------------------- */

#include <stdint.h>
#include <math.h>

#include "constants.h"
#include "utils.h"

/* Data types --------------------------------------------------------------- */

typedef struct
{
    float x;
    float y;
} Coords;

/* Function prototypes ------------------------------------------------------ */

/**
 * @brief COORDS compute euclidean distance between two 2D points.
 * 
 * @param a Point 1
 * @param b Point 2
 * @return uint8_t Distance
 */
static inline uint8_t coords_get_distance(Coords *a, Coords *b)
{
    return sqrtf(POW2(a->x - b->x) + POW2(a->y - b->y)) * DISTANCE_MULTIPLIER;
}

#endif /* COORDS_H */

/* -------------------------------------------------------------------------- */