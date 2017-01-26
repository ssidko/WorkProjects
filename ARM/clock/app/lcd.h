#ifndef _LCD_H
#define _LCD_H

#include <stdint.h>

typedef struct {
	uint32_t width;
	uint32_t height;

	void (*DrawPixel) (uint32_t x, uint32_t y, uint32_t colour);
} LCD;

#endif // _LCD_H
