/* Header guard ------------------------------------------------------------- */

#ifndef COORDS_H
#define COORDS_H

/* Data types --------------------------------------------------------------- */

typedef struct
{
    float x;
    float y;
} Coords;

/* Function prototypes ------------------------------------------------------ */

uint8_t coords_compute_distance(Coords *a, Coords *b);

#endif /* COORDS_H */

/* -------------------------------------------------------------------------- */