/* Includes ----------------------------------------------------------------- */

#include "sound.h"

/* Function definitions ----------------------------------------------------- */

void sound_init(void)
{
	// pinMode(SOUND_PIN, OUTPUT);

	// TCCR2A = (1 << WGM21); // CTC
	// TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20); // prescaler 1024
	// OCR2A = 112 - 1; // 16000000 / 1024 / 112 -> 139,5 Hz
	// TIMSK2 = (1 << OCIE2A);
}

void playSound(const uint8_t *snd, uint8_t len)
{
	// snd_ptr = reinterpret_cast<uint16_t>(snd);
	// snd_len = len;
	// sound = true;
}

// Set the frequency that we will get on pin OCR1A
void setFrequency(uint16_t freq)
{
	// uint32_t requiredDivisor = (F_CPU / 2) / (uint32_t)freq;

	// uint16_t prescalerVal;
	// uint8_t prescalerBits;
	// if (requiredDivisor < 65536UL) {
	// 	prescalerVal = 1;
	// 	prescalerBits = 1; // prescaler 1
	// } else if (requiredDivisor < 8 * 65536UL) {
	// 	prescalerVal = 8;
	// 	prescalerBits = 2; // prescaler 8
	// } else if (requiredDivisor < 64 * 65536UL) {
	// 	prescalerVal = 64;
	// 	prescalerBits = 3; // prescaler 64
	// } else if (requiredDivisor < 256 * 65536UL) {
	// 	prescalerVal = 256;
	// 	prescalerBits = 4; // prescaler 256
	// } else {
	// 	prescalerVal = 1024;
	// 	prescalerBits = 5; // prescaler 1024
	// }

	// uint16_t top =
	// 	((requiredDivisor + (prescalerVal / 2)) / prescalerVal) - 1;
	// TCCR1A = _BV(COM1A0) /*+ _BV(COM1B0)*/;
	// TCCR1B = (1 << WGM12) | prescalerBits; // CTC
	// TCCR1C = _BV(FOC1A);
	// OCR1A = top;
}

void off(void)
{
	// TCCR1A = 0;
}

// ISR(TIMER2_COMPA_vect)
// {
// 	if (sound) {
// 		if (idx++ < snd_len) {
// 			uint16_t freq =
// 				1192030 /
// 				(60 * (uint16_t)pgm_read_byte(snd_ptr +
// 							      idx)); // 1193181
// 			setFrequency(freq);
// 		} else {
// 			idx = 0;
// 			off();
// 			sound = false;
// 		}
// 	}
// }

/* -------------------------------------------------------------------------- */