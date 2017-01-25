#include <stdio.h>
#include "stm32f1xx.h"
#include "stm32f103xb.h"
#include "delay.h"
#include "gpio.h"
#include "spi.h"
#include "ssd1306.h"

#define LED_PIN							Pin12

void HW_Init()
{
	SysTick_Config(SystemCoreClock/1000 - 1);

	// Port-C clock ENABLE
	GPIOC_ClockEnable();

	// LED pin configure
	// General purpose output Open-drain
	// Output mode, max speed 2 MHz
	GPIOC->CRH &= ~(GPIO_CRH_MODE12 | GPIO_CRH_CNF12);
	GPIOC->CRH |= (Out_2Mhz|Out_OpenDrain) << GPIO_CRH_MODE12_Pos;

	ssd1306_Init(128, 32);
}

#include "font_6x8.h"

#define LCD_WIDTH						128
#define LCD_HEIGH						32
#define FRAME_BUFFER_SIZE				(LCD_WIDTH*LCD_HEIGH/8)

uint8_t frame_buffer[FRAME_BUFFER_SIZE];
FrameBuffer ssd1306_fb = {frame_buffer, LCD_WIDTH, LCD_HEIGH};

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

void DrawChar(uint32_t x, uint32_t y, uint8_t ch, Font* font, FrameBuffer *fb)
{
	/* bytes per line */
	int bpl = font->width / 8 + ((font->width % 8) ? 1 : 0);
	/* bytes per char */
	int bpc = bpl * font->height;
	uint8_t *c = &font->chars[(ch - font->offset)*bpc];
	uint8_t colour = 0;

	for (int yy = 0; yy < font->height; yy++) {
		for (int xx = 0; xx < font->width; xx++) {
			colour = c[yy*bpl + xx / 8] & (1 << (8 - (xx % 8))) ? 1 : 0;
			DrawPixel(x + xx, y + yy, colour, fb);
		}
	}
}

void PrintString(uint32_t x, uint32_t y, uint8_t *str, Font* font, FrameBuffer *fb)
{
	while (*str) {



		str++;
	}
}

typedef struct {
	uint32_t x;
	uint32_t y;
} Position;

int main(void)
{
	SystemCoreClockUpdate();

	HW_Init();

	uint32_t start = 0;
	uint32_t elapsed = 0;

	Position pos = {0,0};

    while(1)
    {

    	//Delay_ms(1000);

    	/*
    	start = GetTickCount();
    	for (int y = 0; y < LCD_HEIGH; y++) {
    		for (int x = 0; x < LCD_WIDTH; x++) {
    			DrawPixel(x, y, 1, &ssd1306_fb);
    			ssd1306_UpdateScreen(ssd1306_fb.buff, FRAME_BUFFER_SIZE);
    		}
    	}
    	elapsed = GetTickCount() - start;

    	start = GetTickCount();
    	for (int y = 0; y < LCD_HEIGH; y++) {
    		for (int x = 0; x < LCD_WIDTH; x++) {
    			DrawPixel(x, y, 0, &ssd1306_fb);
    			ssd1306_UpdateScreen(ssd1306_fb.buff, FRAME_BUFFER_SIZE);
    		}
    	}
    	elapsed = GetTickCount() - start;
    	*/

    	ssd1306_Wakeup();

    	for (uint32_t i = 0; i < FRAME_BUFFER_SIZE; i++) {
    		frame_buffer[i] = 0x00;
    	}
    	ssd1306_UpdateScreen(ssd1306_fb.buff, FRAME_BUFFER_SIZE);

    	pos.x = 0;
    	pos.y = 0;
    	ssd1306_ClearScreen(FRAME_BUFFER_SIZE);
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
    				ssd1306_UpdateScreen(ssd1306_fb.buff, FRAME_BUFFER_SIZE);
    			}
    		}
        	DrawChar(pos.x, pos.y, symbol, &fnt6x8, &ssd1306_fb);
        	ssd1306_UpdateScreen(ssd1306_fb.buff, FRAME_BUFFER_SIZE);
        	pos.x += fnt6x8.width;
    	}

    	GPIO_ToglePin(GPIOC, LED_PIN);
    	Delay_ms(5000);

    	pos.x = 0;
    	pos.y = 0;
    	ssd1306_ClearScreen(FRAME_BUFFER_SIZE);
    	for (uint32_t symbol = 'À'; symbol <= 'ÿ'; symbol++) {
    		if (pos.x + fnt6x8.width > ssd1306_fb.width) {
    			pos.x = 0;
    			pos.y += fnt6x8.height;
    			if (pos.y + fnt6x8.height > ssd1306_fb.height) {
    				pos.y = 0;
    				Delay_ms(2000);
    				for (uint32_t i = 0; i < FRAME_BUFFER_SIZE; i++) {
    					frame_buffer[i] = 0x00;
    				}
    				ssd1306_UpdateScreen(ssd1306_fb.buff, FRAME_BUFFER_SIZE);
    			}
    		}
        	DrawChar(pos.x, pos.y, symbol, &fnt6x8, &ssd1306_fb);
        	ssd1306_UpdateScreen(ssd1306_fb.buff, FRAME_BUFFER_SIZE);
        	pos.x += fnt6x8.width;
    	}

    	Delay_ms(5000);

    	ssd1306_Sleep();
    	Delay_ms(5000);
    }
}













































