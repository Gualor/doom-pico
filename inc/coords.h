#ifndef TYPES_H
#define TYPES_H

typedef struct
{
	float x;
	float y;
} Coords;

Coords coords_create(float x, float y);
uint8_t coords_compute_distance(Coords *a, Coords *b);

#endif /* TYPES_H */
