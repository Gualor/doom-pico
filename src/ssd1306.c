#include "ssd1306.h"

bool ssd1306_begin(uint8_t switchvcc, uint8_t i2caddr)
{
}

void ssd1306_display(void)
{
}

void ssd1306_clearDisplay(void)
{
}
void ssd1306_invertDisplay(bool i)
{
}

void ssd1306_drawPixel(int16_t x, int16_t y, uint16_t color)
{
}

void ssd1306_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
}

bool ssd1306_getPixel(int16_t x, int16_t y)
{
}

uint8_t *ssd1306_getBuffer(void)
{
}

void ssd1306_clearRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
}

void ssd1306_drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w,
						int16_t h, uint16_t color)
{
}

void ssd1306_drawFastVLineInternal(int16_t x, int16_t y, int16_t h,
								   uint16_t color)
{
}

void ssd1306_command1(uint8_t c)
{
}

void ssd1306_commandList(const uint8_t *c, uint8_t n)
{
}