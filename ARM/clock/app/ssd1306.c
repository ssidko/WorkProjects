#include "ssd1306.h"
#include "spi.h"
#include "gpio.h"
#include "delay.h"

#define SSD1306_RESET_HIGH()					(GPIO_PinSet(SSD1306_RESET_PORT, SSD1306_RESET_PIN))
#define SSD1306_RESET_LOW()						(GPIO_PinReset(SSD1306_RESET_PORT, SSD1306_RESET_PIN))

#define SSD1306_DC_HIGH()						(GPIO_PinSet(SSD1306_DC_PORT, SSD1306_DC_PIN))
#define SSD1306_DC_LOW()						(GPIO_PinReset(SSD1306_DC_PORT, SSD1306_DC_PIN))

#define SSD1306_CS_HIGH()						(GPIO_PinSet(SSD1306_CS_PORT, SSD1306_CS_PIN))
#define SSD1306_CS_LOW()						(GPIO_PinReset(SSD1306_CS_PORT, SSD1306_CS_PIN))


void SSD1306_GpioInit(void)
{
	// CS			PA-1
	// RESET		PA-2
	// DC			PA-3
	// SCK			PA-5
	// MOSI			PA-7

	// PA-1 - LCD_CS; PA-2 - LCD_RESET; PA-3 - LCD_DC
	// MODE: 0b0010: Output mode, max speed 2 MHz.
	// CNF:  0b0000: General purpose output push-pull
	GPIOA->CRL &= ~((0x0F << (Pin2 * 4)) | (0x0F << (Pin3 * 4)) | (0x0F << (Pin1 * 4)));
	GPIOA->CRL |= ((Out_2Mhz|Out_PushPull) << (Pin1 * 4)) |
				  ((Out_2Mhz|Out_PushPull) << (Pin2 * 4)) |
				  ((Out_2Mhz|Out_PushPull) << (Pin3 * 4));
}

void SSD1306_Reset(void)
{
	SSD1306_RESET_HIGH();
	Delay_ms(10);
	SSD1306_RESET_LOW();
	Delay_ms(1);
	SSD1306_RESET_HIGH();
	Delay_ms(10);
}

void SSD1306_Init(uint32_t width, uint32_t height)
{
	SSD1306_GpioInit();

	SSD1306_Reset();

	SSD1306_CS_LOW();
	SSD1306_DC_LOW();

	// sleep
	SPI_SendByte(SPI1, 0xAF);

	// Set MUX Ratio
	SPI_SendByte(SPI1, 0xA8);
	SPI_SendByte(SPI1, 0x1F);
	// Set Display Offset
	SPI_SendByte(SPI1, 0xD3);
	SPI_SendByte(SPI1, 0x00);
	// Set Display start line
	SPI_SendByte(SPI1, 0x40);
	// Set Segment re-map
	SPI_SendByte(SPI1, 0xA1);
	// Set COM Output Scan Direction
	SPI_SendByte(SPI1, 0xC8);
	// Set COM Pins Hardware Configuration
	SPI_SendByte(SPI1, 0xDA);
	SPI_SendByte(SPI1, 0x02);
	// Set Contrast Control
	SPI_SendByte(SPI1, 0x81);
	SPI_SendByte(SPI1, 0x7F);
	// Disable Entire Display ON
	SPI_SendByte(SPI1, 0xA4);
	// Set Normal Display
	SPI_SendByte(SPI1, 0xA6);
	// Set the Oscillator Frequency, FOSC
	SPI_SendByte(SPI1, 0xD5);
	SPI_SendByte(SPI1, 0x80);

	// Enable charge pump regulator
	SPI_SendByte(SPI1, 0x8d);
	SPI_SendByte(SPI1, 0x14);

	//Set Memory Addressing Mode (20h)
	SPI_SendByte(SPI1, 0x20);
	SPI_SendByte(SPI1, 0x00);

	// Set Column Address
	SPI_SendByte(SPI1, 0x21);
	SPI_SendByte(SPI1, 0x00);
	SPI_SendByte(SPI1, 0x7F);

	// Set Page Address
	SPI_SendByte(SPI1, 0x22);
	SPI_SendByte(SPI1, 0x00);
	SPI_SendByte(SPI1, 0x03);

	// Display ON
	SPI_SendByte(SPI1, 0xAF);

	Delay_ms(100);

	SSD1306_DC_HIGH();
	SSD1306_CS_HIGH();
}

void SSD1306_Sleep(void)
{
	SSD1306_CS_LOW();
	SSD1306_DC_LOW();

	SPI_SendByte(SPI1, 0xAE);

	SSD1306_DC_HIGH();
	SSD1306_CS_HIGH();
}

void SSD1306_Wakeup(void)
{
	SSD1306_CS_LOW();
	SSD1306_DC_LOW();

	SPI_SendByte(SPI1, 0xAF);

	SSD1306_DC_HIGH();
	SSD1306_CS_HIGH();
}

void SSD1306_InverseOn(void)
{
	SSD1306_CS_LOW();
	SSD1306_DC_LOW();

	SPI_SendByte(SPI1, 0xA7);

	SSD1306_DC_HIGH();
	SSD1306_CS_HIGH();
}

void SSD1306_InverseOff(void)
{
	SSD1306_CS_LOW();
	SSD1306_DC_LOW();

	SPI_SendByte(SPI1, 0xA6);

	SSD1306_DC_HIGH();
	SSD1306_CS_HIGH();
}

void SSD1306_HorisontalMode(void)
{
	SSD1306_CS_LOW();
	SSD1306_DC_LOW();

	SPI_SendByte(SPI1, 0x20);

	SSD1306_DC_HIGH();
	SSD1306_CS_HIGH();
}

void SSD1306_VerticalMode(void)
{
	SSD1306_CS_LOW();
	SSD1306_DC_LOW();

	SPI_SendByte(SPI1, 0x21);

	SSD1306_DC_HIGH();
	SSD1306_CS_HIGH();
}

void SSD1306_RowRemapOn(void)
{
	SSD1306_CS_LOW();
	SSD1306_DC_LOW();

	SPI_SendByte(SPI1, 0xC8);

	SSD1306_DC_HIGH();
	SSD1306_CS_HIGH();
}

void SSD1306_RowRemapOff(void)
{
	SSD1306_CS_LOW();
	SSD1306_DC_LOW();

	SPI_SendByte(SPI1, 0xC0);

	SSD1306_DC_HIGH();
	SSD1306_CS_HIGH();
}

void SSD1306_ColumnRemapOn(void)
{
	SSD1306_CS_LOW();
	SSD1306_DC_LOW();

	SPI_SendByte(SPI1, 0xA1);

	SSD1306_DC_HIGH();
	SSD1306_CS_HIGH();
}

void SSD1306_ColumnRemapOff(void)
{
	SSD1306_CS_LOW();
	SSD1306_DC_LOW();

	SPI_SendByte(SPI1, 0xA0);

	SSD1306_DC_HIGH();
	SSD1306_CS_HIGH();
}

void SSD1306_UpdateScreen(uint8_t *screen_buffer, uint32_t size)
{
	SSD1306_CS_LOW();
	SSD1306_DC_HIGH();
	for (uint32_t i = 0; i < size; i++) {
		SPI_SendByte(SPI1, *screen_buffer++);
	}
	SSD1306_CS_HIGH();
}

void SSD1306_ClearScreen(uint32_t size)
{
	SSD1306_CS_LOW();
	SSD1306_DC_HIGH();
	for (uint32_t i = 0; i < size; i++) {
		SPI_SendByte(SPI1, 0x00);
	}
	SSD1306_CS_HIGH();
}















