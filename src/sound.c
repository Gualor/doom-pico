/* Includes ----------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>

#include "sound.h"
#include "platform.h"

/* Global variables --------------------------------------------------------- */

static uint8_t *sound_ptr;
static uint8_t sound_len;
static uint8_t sound_idx;
static uint32_t sound_t0;

/* Function definitions ----------------------------------------------------- */

/**
 * @brief SOUND initialize audio player state.
 *
 */
void sound_init(void)
{
    sound_ptr = NULL;
    sound_len = 0;
}

/**
 * @brief SOUND update sound and execute platform audio player.
 *
 * @param snd Sound byte array
 * @param len Byte length of sound
 */
void sound_play(const uint8_t *snd, uint8_t len)
{
    // Assign new sound effect
    sound_ptr = (uint8_t *)snd;
    sound_len = len;

    // Init sound player state
    sound_idx = 0;
    sound_t0 = platform_millis();

    // Execute platform audio player
    platform_audio_play();
}

/**
 * @brief SOUND get a new frequency from current sound every (1/140)s period,
 * otherwise return the same one again. Return zero if sound is finished.
 *
 * NOTE: Sounds use inverse frequency format, for more information see:
 * 1. http://fabiensanglard.net/gebbwolf3d_v2.1.pdf chapter 4.9.5
 * 2. http://www.shikadi.net/moddingwiki/Inverse_Frequency_Sound_format
 * 3. http://www.shikadi.net/moddingwiki/AudioT_Format
 *
 * @return uint16_t Next sound frequency
 */
uint16_t sound_get_frequency(void)
{
    // Return zero if sound is finished
    if (sound_idx >= sound_len)
        return 0;

    // After (1/140)s period increment sound index
    uint32_t sound_t1 = platform_millis();
    if ((sound_t1 - sound_t0) > 7)
    {
        sound_idx++;
        sound_t0 = sound_t1;
    }

    // Get frequency value from byte encoding
    return 1192030 / (60 * (uint16_t)sound_ptr[sound_idx]);
}

/* -------------------------------------------------------------------------- */