/*
   For infos how this works see
   http://fabiensanglard.net/gebbwolf3d_v2.1.pdf chapter 4.9.5
   and
   http://www.shikadi.net/moddingwiki/Inverse_Frequency_Sound_format
   and
   http://www.shikadi.net/moddingwiki/AudioT_Format
*/

#ifndef _sound_h
#define _sound_h

#include <stdint.h>
#include <stdbool.h>

#include "constants.h"

static const uint8_t GET_KEY_SND_LEN = 90;
static const uint8_t SHOOT_SND_LEN = 27;
static const uint8_t shoot_snd[] = {0x10, 0x10, 0x10, 0x6e, 0x2a, 0x20, 0x28, 0x28, 0x9b, 0x28, 0x20, 0x20, 0x21, 0x57, 0x20, 0x20, 0x20, 0x67, 0x20, 0x20, 0x29, 0x20, 0x73, 0x20, 0x20, 0x20, 0x89};
static const uint8_t get_key_snd[] = {0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x20, 0x20, 0x20, 0x20, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x20, 0x20, 0x20, 0x20, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19};
static const uint8_t HIT_WALL_SND_LEN = 8;
static const uint8_t hit_wall_snd[] = {0x83, 0x83, 0x82, 0x8e, 0x8a, 0x89, 0x86, 0x84};
static const uint8_t WALK1_SND_LEN = 3;
static const uint8_t walk1_snd[] = {0x8f, 0x8e, 0x8e};
static const uint8_t WALK2_SND_LEN = 3;
static const uint8_t walk2_snd[] = {0x84, 0x87, 0x84};
static const uint8_t MEDKIT_SND_LEN = 69;
static const uint8_t medkit_snd[] = {0x55, 0x20, 0x3a, 0x3a, 0x3a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x33, 0x33, 0x33, 0x33, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x26, 0x26, 0x26, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x16, 0x16, 0x16, 0x16, 0x16, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x16, 0x16, 0x16, 0x16, 0x16, 0x16, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15};

void sound_init(void);
void sound_play(const uint8_t *snd, uint8_t len);
void setFrequency(uint16_t freq);
void off(void);

extern bool sound;

#endif
