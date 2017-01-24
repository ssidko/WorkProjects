#include <stdio.h>
#include "stm32f1xx.h"
#include "stm32f103xb.h"
#include "delay.h"
#include "gpio.h"
#include "spi.h"

#define LED_PIN							Pin12

void LCD_Reset(void)
{
	GPIO_PinSet(GPIOA, 2);
	Delay_ms(10);
	GPIO_PinReset(GPIOA, 2);
	Delay_ms(10);
	GPIO_PinSet(GPIOA, 2);
}

void HW_Init()
{
	// Port-A clock ENABLE
	GPIOA_ClockEnable();

	// Port-C clock ENABLE
	GPIOC_ClockEnable();

	// LED pin configure
	// General purpose output Open-drain
	// Output mode, max speed 2 MHz
	GPIOC->CRH &= ~(GPIO_CRH_MODE12 | GPIO_CRH_CNF12);
	GPIOC->CRH |= (Out_2Mhz|Out_OpenDrain) << GPIO_CRH_MODE12_Pos;

	// Alternate function I/O clock enable
	//SET_BIT(RCC->APB2ENR, RCC_APB2ENR_AFIOEN);
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

	//SPI-1 clock ENABLE
	SPI1_ClockEnable();

	// PA-5 - LCD_SCK; PA-7 - LCD_MOSI
	// MODE: 0b0011: Output mode, max speed 50 MHz.
	// CNF:  0b1000: Alternate function output Push-pull
	GPIOA->CRL &= ~((0x0F << (Pin5 * 4)) | (0x0F << (Pin7 * 4)));
	GPIOA->CRL |= ((Out_50Mhz|Out_AF_PushPull) << (Pin5 * 4)) |
				  ((Out_50Mhz|Out_AF_PushPull) << (Pin7 * 4));

	// PA-2 - LCD_RESET; PA-3 - LCD_DC
	// MODE: 0b0010: Output mode, max speed 2 MHz.
	// CNF:  0b0000: General purpose output push-pull
	GPIOA->CRL &= ~((0x0F << (Pin2 * 4)) | (0x0F << (Pin3 * 4)));
	GPIOA->CRL |= ((Out_2Mhz|Out_AF_PushPull) << (Pin2 * 4)) |
				  ((Out_2Mhz|Out_AF_PushPull) << (Pin3 * 4));

	// LCD_RESET to high
	GPIO_PinSet(GPIOA, 2);

	SysTick_Config(SystemCoreClock/1000 - 1);

	SPI1->CR1 |= 0x4004;
	//SPI1->CR1 |= 0x4007;

	// set CR1->SPE ENABLED
	SPI_Enable(SPI1);

	// LCD init sequence
	Delay_ms(10);

	LCD_Reset();
	Delay_ms(10);

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

int main(void)
{
	SystemCoreClockUpdate();

	HW_Init();

    while(1)
    {
    	GPIO_ToglePin(GPIOC, LED_PIN);
    	Delay_ms(1000);
    }
}













































