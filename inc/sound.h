/*
   For infos how this works see
   http://fabiensanglard.net/gebbwolf3d_v2.1.pdf chapter 4.9.5
   and
   http://www.shikadi.net/moddingwiki/Inverse_Frequency_Sound_format
   and
   http://www.shikadi.net/moddingwiki/AudioT_Format
*/

/* Header guard ------------------------------------------------------------- */

#ifndef SOUND_H
#define SOUND_H

/* Includes ----------------------------------------------------------------- */

#include <stdint.h>
#include <stdbool.h>

#include "constants.h"

/* Function protypes -------------------------------------------------------- */

void sound_init(void);
void playSound(const uint8_t *snd, uint8_t len);
void setFrequency(uint16_t freq);
void off(void);

/* Global variables --------------------------------------------------------- */

// Shoot
#define SHOOT_SND_LEN 27
#define SHOOT_SND                                                                                                                                                        \
	(const uint8_t[])                                                                                                                                                    \
	{                                                                                                                                                                    \
		0x10, 0x10, 0x10, 0x6e, 0x2a, 0x20, 0x28, 0x28, 0x9b, 0x28, 0x20, 0x20, 0x21, 0x57, 0x20, 0x20, 0x20, 0x67, 0x20, 0x20, 0x29, 0x20, 0x73, 0x20, 0x20, 0x20, 0x89 \
	}

// Get key
#define GET_KEY_SND_LEN 90
#define GET_KEY_SND                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                \
	(const uint8_t[])                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              \
	{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              \
		0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x20, 0x20, 0x20, 0x20, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x20, 0x20, 0x20, 0x20, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19 \
	}

// Hit wall
#define HIT_WALL_SND_LEN 8
#define HIT_WALL_SND                                   \
	(const uint8_t[])                                  \
	{                                                  \
		0x83, 0x83, 0x82, 0x8e, 0x8a, 0x89, 0x86, 0x84 \
	}

// Walk 1
#define WALK1_SND_LEN 3
#define WALK1_SND        \
	(const uint8_t[])    \
	{                    \
		0x8f, 0x8e, 0x8e \
	}

// Walk 2
#define WALK2_SND_LEN 3
#define WALK2_SND        \
	(const uint8_t[])    \
	{                    \
		0x84, 0x87, 0x84 \
	}

// Medkit
#define MEDKIT_SND_LEN 69
#define MEDKIT_SND                                                                                                                                                                                                                                                                                                                                                                                                                                \
	(const uint8_t[])                                                                                                                                                                                                                                                                                                                                                                                                                             \
	{                                                                                                                                                                                                                                                                                                                                                                                                                                             \
		0x55, 0x20, 0x3a, 0x3a, 0x3a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x33, 0x33, 0x33, 0x33, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x26, 0x26, 0x26, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x16, 0x16, 0x16, 0x16, 0x16, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x16, 0x16, 0x16, 0x16, 0x16, 0x16, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, \
	}

// volatile bool sound = false;
extern volatile bool sound;

#endif /* SOUND_H */

/* -------------------------------------------------------------------------- */