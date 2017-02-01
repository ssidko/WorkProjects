#ifndef _SSD1306_H
#define _SSD1306_H

#include <stdint.h>
#include "stm32f1xx.h"
#include "gpio.h"

#define SSD1306_CMD_PORT							GPIOA

#define SSD1306_CS_PORT								SSD1306_CMD_PORT
#define SSD1306_CS_PIN								Pin1
#define SSD1306_RESET_PORT							SSD1306_CMD_PORT
#define SSD1306_RESET_PIN							Pin2
#define SSD1306_DC_PORT								SSD1306_CMD_PORT
#define SSD1306_DC_PIN								Pin3
#define SSD1306_SCK_PORT							SSD1306_CMD_PORT
#define SSD1306_SCK_PIN								Pin5
#define SSD1306_MOSI_PORT							SSD1306_CMD_PORT
#define SSD1306_MOSI_PIN							Pin7


typedef struct {
	uint8_t *buff;
	uint32_t width;
	uint32_t height;
} FrameBuffer;

void SSD1306_Init(uint32_t width, uint32_t height);
void SSD1306_Reset(void);
void SSD1306_Sleep(void);
void SSD1306_Wakeup(void);
void SSD1306_InverseOn(void);
void SSD1306_InverseOff(void);
void SSD1306_HorisontalMode(void);
void SSD1306_VerticalMode(void);
void SSD1306_RowRemapOn(void);
void SSD1306_RowRemapOff(void);
void SSD1306_ColumnRemapOn(void);
void SSD1306_ColumnRemapOff(void);
void SSD1306_UpdateScreen(uint8_t *screen_buffer, uint32_t size);
void SSD1306_ClearScreen(uint32_t size);

#endif // _SSD1306_H
