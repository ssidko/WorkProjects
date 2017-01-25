#ifndef _SSD1306_H
#define _SSD1306_H

#include <stdint.h>
#include "stm32f1xx.h"

typedef struct {
	uint32_t width;
	uint32_t height;
} ssd1306_driver;

typedef struct {
	uint8_t *buff;
	uint32_t width;
	uint32_t height;
} FrameBuffer;

void ssd1306_Init(uint32_t width, uint32_t height);
void ssd1306_Sleep(void);
void ssd1306_Wakeup(void);
void ssd1306_UpdateScreen(uint8_t *screen_buffer, uint32_t size);
void ssd1306_ClearScreen(uint32_t size);

#endif // _SSD1306_H
