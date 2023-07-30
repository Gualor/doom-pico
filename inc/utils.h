/* Header guard ------------------------------------------------------------- */

#ifndef UTILS_H
#define UTILS_H

/* Definitions -------------------------------------------------------------- */

#define SWAP(a, b) do { typeof(a) temp = a; a = b; b = temp; } while (0)
#define SIGN(a, b) ((a) > (b) ? 1 : ((b) > (a) ? -1 : 0))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define POW2(a) ((a) * (a))
#define BIT_MASK (uint8_t[8]){128, 64, 32, 16, 8, 4, 2, 1}
#define READ_BIT(byte, pos) (byte & *(BIT_MASK + pos) ? 1 : 0)
#define PI 3.14159265358979323846f

/* Function prototypes ------------------------------------------------------ */

uint32_t millis(void);
void delay(uint32_t ms);

#endif /* UTILS_H */

/* -------------------------------------------------------------------------- */