/* Includes ----------------------------------------------------------------- */

#include <stdlib.h>

#include "sound.h"
#include "platform.h"

/* Global variables --------------------------------------------------------- */

volatile bool sound = false;

static uint8_t idx = 0;
static uint16_t *snd_ptr = NULL;
static uint8_t snd_len = 0;

/* Function definitions ----------------------------------------------------- */

void sound_init(void)
{
	platform_audio_init();

	idx = 0;
	sound = false;
	snd_ptr = NULL;
	snd_len = 0;
}

void sound_play(const uint8_t *snd, uint8_t len)
{
	platform_audio_play_start();

	snd_ptr = (uint16_t *)(snd);
	snd_len = len;
	sound = true;
}

// Set the frequency that we will get on pin OCR1A
void sound_set_frequency(uint16_t freq)
{
	uint32_t requiredDivisor = (F_CPU / 2) / (uint32_t)freq;

	uint16_t prescalerVal;
	uint8_t prescalerBits;
	if (requiredDivisor < 65536UL)
	{
		prescalerVal = 1;
		prescalerBits = 1; // prescaler 1
	}
	else if (requiredDivisor < 8 * 65536UL)
	{
		prescalerVal = 8;
		prescalerBits = 2; // prescaler 8
	}
	else if (requiredDivisor < 64 * 65536UL)
	{
		prescalerVal = 64;
		prescalerBits = 3; // prescaler 64
	}
	else if (requiredDivisor < 256 * 65536UL)
	{
		prescalerVal = 256;
		prescalerBits = 4; // prescaler 256
	}
	else
	{
		prescalerVal = 1024;
		prescalerBits = 5; // prescaler 1024
	}

	uint16_t top = ((requiredDivisor + (prescalerVal / 2)) / prescalerVal) - 1;
	//   TCCR1A = _BV(COM1A0) /*+ _BV(COM1B0)*/;
	//   TCCR1B = (1 << WGM12) | prescalerBits;  // CTC
	//   TCCR1C = _BV(FOC1A);
	//   OCR1A = top;
}

void sound_stop(void)
{
	platform_audio_play_stop();
}

// ISR(TIMER2_COMPA_vect) {
//   if (sound) {
//     if (idx++ < snd_len) {
//       uint16_t freq = 1192030 / (60 * (uint16_t)snd_ptr[idx])); // 1193181
//       sound_set_frequency(freq);
//     } else {
//       idx = 0;
//       sound_stop();
//       sound = false;
//     }
//   }
// }

/* -------------------------------------------------------------------------- */