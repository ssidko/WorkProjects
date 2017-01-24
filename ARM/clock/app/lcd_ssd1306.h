#ifndef _SSD1306_H
#define _SSD1306_H

#include <stdint.h>
#include "stm32f1xx.h"

typedef struct {
	uint32_t width;
	uint32_t height;

} ssd1306_driver;

void ssd1306_GpioInit(void);
void ssd1306_Init(uint32_t width, uint32_t height);

#endif // _SSD1306_H
