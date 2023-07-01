#include <time.h>
#include <stdio.h>
#include "platform.h"

volatile uint32_t init_clock;

uint32_t millis(void)
{
	return ((clock() - init_clock) * 1000) / CLOCKS_PER_SEC;
}

void delay(uint32_t ms)
{
	uint32_t t0 = millis();
	while ((millis() - t0) < ms)
	{
		printf("%d\t%d\t%d\n", millis(), t0, ms);
	};
}