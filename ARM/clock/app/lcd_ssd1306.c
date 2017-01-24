#include "lcd_ssd1306.h"
#include "spi.h"
#include "gpio.h"

void ssd1306_GpioInit(void)
{

}

void ssd1306_Init(uint32_t width, uint32_t height)
{
	// LCD_DC to low for command
	GPIO_PinReset(GPIOA, 3);

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

	GPIO_PinSet(GPIOA, 3);
}
