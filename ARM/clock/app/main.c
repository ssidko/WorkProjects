#include <stdio.h>
#include <stdlib.h>
#include "stm32f1xx.h"
#include "stm32f103xb.h"
#include "delay.h"
#include "font_6x8.h"

#include "hardware.h"

#define FRAME_BUFFER_SIZE				(LCD_WIDTH*LCD_HEIGHT/8)

uint8_t frame_buffer[FRAME_BUFFER_SIZE];
FrameBuffer ssd1306_fb = {frame_buffer, LCD_WIDTH, LCD_HEIGHT};

void DrawPixel(uint32_t x, uint32_t y, uint32_t colour, FrameBuffer *fb)
{
	if ( (x >= fb->width) || (y >= fb->height) ) {
		return;
	}

	uint32_t row = y / 8;
	uint32_t bit = y % 8;

	if (colour) {
		fb->buff[row * fb->width + x] |= 1 << bit;
	} else {
		fb->buff[row * fb->width + x] &= ~(1 << bit);
	}
}

void DrawChar(uint32_t x, uint32_t y, char ch, Font* font, FrameBuffer *fb)
{
	/* bytes per line */
	int bpl = font->width / 8 + ((font->width % 8) ? 1 : 0);
	/* bytes per character */
	int bpc = bpl * font->height;
	uint8_t *c = &font->chars[(ch - font->offset)*bpc];
	uint8_t colour = 0;

	for (int yy = 0; yy < font->height; yy++) {
		for (int xx = 0; xx < font->width; xx++) {
			colour = c[yy*bpl + xx / 8] & (1 << ((xx % 8))) ? 1 : 0;
			DrawPixel(x + xx, y + yy, colour, fb);
		}
	}
}

void PrintString(uint32_t x, uint32_t y, char *str, Font* font, FrameBuffer *fb)
{
	while (*str) {
		if ((x + font->width <= fb->width) && (y + font->height <= fb->height)) {
			DrawChar(x, y, *str, font, fb);
			x += font->width;
		} else {
			break;
		}
		str++;
	}
}

typedef struct {
	uint32_t x;
	uint32_t y;
} Position;

void DemoApp1(void);

int main(void)
{
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock/1000 - 1);

	HW_Init();

	/*clear screen*/
	for (uint32_t i = 0; i < FRAME_BUFFER_SIZE; i++) {
		frame_buffer[i] = 0x00;
	}
	SSD1306_UpdateScreen(ssd1306_fb.buff, FRAME_BUFFER_SIZE);
	/*clear screen*/


	char *str = malloc(32);

	sprintf(str, "SysClk: %u MHz", SystemCoreClock/1000000);
	PrintString(0, 0, str, &fnt6x8, &ssd1306_fb);
	PrintString(0, 8, str, &fnt6x8, &ssd1306_fb);
	PrintString(0, 16, str, &fnt6x8, &ssd1306_fb);
	PrintString(0, 24, str, &fnt6x8, &ssd1306_fb);
	SSD1306_UpdateScreen(ssd1306_fb.buff, FRAME_BUFFER_SIZE);

	free(str);

	Delay_ms(1000);

    while(1)
    {
    	LED_TOGLE();
    	Delay_ms(1000);
    	DemoApp1();
    }
}


void DemoApp1(void)
{
	Position pos = {0,0};

	SSD1306_Wakeup();

	for (uint32_t i = 0; i < FRAME_BUFFER_SIZE; i++) {
		frame_buffer[i] = 0x00;
	}
	SSD1306_UpdateScreen(ssd1306_fb.buff, FRAME_BUFFER_SIZE);

	PrintString(50, 10, "Hello", &fnt6x8, &ssd1306_fb);
	SSD1306_UpdateScreen(ssd1306_fb.buff, FRAME_BUFFER_SIZE);
	Delay_ms(1000);

	for (int i = 0; i < 4; i++) {
		SSD1306_InverseOn();
		Delay_ms(500);
		SSD1306_InverseOff();
		Delay_ms(500);
	}

	/*clear screen*/
	for (uint32_t i = 0; i < FRAME_BUFFER_SIZE; i++) {
		frame_buffer[i] = 0x00;
	}
	SSD1306_UpdateScreen(ssd1306_fb.buff, FRAME_BUFFER_SIZE);
	/*clear screen*/

	PrintString(5, 10, "Демонстрация шрифта", &fnt6x8, &ssd1306_fb);
	SSD1306_UpdateScreen(ssd1306_fb.buff, FRAME_BUFFER_SIZE);
	Delay_ms(2000);

	/*clear screen*/
	for (uint32_t i = 0; i < FRAME_BUFFER_SIZE; i++) {
		frame_buffer[i] = 0x00;
	}
	SSD1306_UpdateScreen(ssd1306_fb.buff, FRAME_BUFFER_SIZE);
	/*clear screen*/


	PrintString(10, 10, "Английские буквы", &fnt6x8, &ssd1306_fb);
	PrintString(15, 18, "и спец символы", &fnt6x8, &ssd1306_fb);
	SSD1306_UpdateScreen(ssd1306_fb.buff, FRAME_BUFFER_SIZE);
	Delay_ms(2000);

	/*clear screen*/
	for (uint32_t i = 0; i < FRAME_BUFFER_SIZE; i++) {
		frame_buffer[i] = 0x00;
	}
	SSD1306_UpdateScreen(ssd1306_fb.buff, FRAME_BUFFER_SIZE);
	/*clear screen*/


	pos.x = 0;
	pos.y = 0;
	SSD1306_ClearScreen(FRAME_BUFFER_SIZE);
	for (uint32_t symbol = '('; symbol <= '}'; symbol++) {
		if (pos.x + fnt6x8.width > ssd1306_fb.width) {
			pos.x = 0;
			pos.y += fnt6x8.height;
			if (pos.y + fnt6x8.height > ssd1306_fb.height) {
				pos.y = 0;
				Delay_ms(2000);
				for (uint32_t i = 0; i < FRAME_BUFFER_SIZE; i++) {
					frame_buffer[i] = 0x00;
				}
				SSD1306_UpdateScreen(ssd1306_fb.buff, FRAME_BUFFER_SIZE);
			}
		}
    	DrawChar(pos.x, pos.y, symbol, &fnt6x8, &ssd1306_fb);
    	SSD1306_UpdateScreen(ssd1306_fb.buff, FRAME_BUFFER_SIZE);
    	pos.x += fnt6x8.width;
	}

	GPIO_ToglePin(GPIOC, LED_PIN);
	Delay_ms(5000);

	/*clear screen*/
	for (uint32_t i = 0; i < FRAME_BUFFER_SIZE; i++) {
		frame_buffer[i] = 0x00;
	}
	SSD1306_UpdateScreen(ssd1306_fb.buff, FRAME_BUFFER_SIZE);
	/*clear screen*/


	PrintString(40, 10, "Кирилица", &fnt6x8, &ssd1306_fb);
	SSD1306_UpdateScreen(ssd1306_fb.buff, FRAME_BUFFER_SIZE);
	Delay_ms(2000);

	/*clear screen*/
	for (uint32_t i = 0; i < FRAME_BUFFER_SIZE; i++) {
		frame_buffer[i] = 0x00;
	}
	SSD1306_UpdateScreen(ssd1306_fb.buff, FRAME_BUFFER_SIZE);
	/*clear screen*/

	pos.x = 0;
	pos.y = 0;
	for (uint32_t symbol = 'А'; symbol <= 'я'; symbol++) {
		if (pos.x + fnt6x8.width > ssd1306_fb.width) {
			pos.x = 0;
			pos.y += fnt6x8.height;
			if (pos.y + fnt6x8.height > ssd1306_fb.height) {
				pos.y = 0;
				Delay_ms(2000);
				for (uint32_t i = 0; i < FRAME_BUFFER_SIZE; i++) {
					frame_buffer[i] = 0x00;
				}
				SSD1306_UpdateScreen(ssd1306_fb.buff, FRAME_BUFFER_SIZE);
			}
		}
    	DrawChar(pos.x, pos.y, symbol, &fnt6x8, &ssd1306_fb);
    	SSD1306_UpdateScreen(ssd1306_fb.buff, FRAME_BUFFER_SIZE);
    	pos.x += fnt6x8.width;
	}
	Delay_ms(5000);

	/*clear screen*/
	for (uint32_t i = 0; i < FRAME_BUFFER_SIZE; i++) {
		frame_buffer[i] = 0x00;
	}
	SSD1306_UpdateScreen(ssd1306_fb.buff, FRAME_BUFFER_SIZE);
	/*clear screen*/

	PrintString(10, 10, "Go to sleep ...", &fnt6x8, &ssd1306_fb);
	SSD1306_UpdateScreen(ssd1306_fb.buff, FRAME_BUFFER_SIZE);
	Delay_ms(2000);

	SSD1306_Sleep();
	Delay_ms(7000);
}















































