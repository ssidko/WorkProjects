#include "hardware.h"
#include "delay.h"

void HW_Init()
{
	GPIOA_ClockEnable();
	GPIOC_ClockEnable();

	// Alternate function I/O clock enable
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

	//
	// LED pin configure
	//
	LED_PORT->CRH &= ~(0x0F << ((LED_PIN - 8) * 4));
	LED_PORT->CRH |= ((Out_2Mhz|Out_PushPull) << ((LED_PIN - 8) * 4));

	//
	// SPI1 initialization for: ssd1306, bmp280 etc.
	//
	SPI1_Init();

	//
	// ssd1306 initialization
	//
	SSD1306_Init(128, 32);

	//
	// bmp280 initialization
	//

	// BMP280_CS pin configure
	BMP280_CS_PORT->CRL &= ~(0x0F << (BMP280_CS_PIN * 4));
	BMP280_CS_PORT->CRL |= ((Out_2Mhz|Out_PushPull) << (BMP280_CS_PIN * 4));

	// Enable BMP280 SPI interface
	GPIO_PinReset(BMP280_CS_PORT, BMP280_CS_PIN);
	Delay_ms(1);
	GPIO_PinSet(BMP280_CS_PORT, BMP280_CS_PIN);

	//
	// TODO: RTC initialization
	//

}

int SPI1_Init(void)
{
	// PA-5 - SCK; PA-6 - MISO; PA-7 - MOSI;
	// MODE: 0b0011: Output mode, max speed 50 MHz.
	// CNF:  0b1000: Alternate function output Push-pull
	GPIOA->CRL &= ~((0x0F << (Pin5 * 4)) | (0x0F << (Pin6 * 4)) | (0x0F << (Pin7 * 4)));
	GPIOA->CRL |= ((Out_50Mhz|Out_AF_PushPull) << (Pin5 * 4)) |
				  ((Out_50Mhz|Out_AF_PushPull) << (Pin6 * 4)) |
				  ((Out_50Mhz|Out_AF_PushPull) << (Pin7 * 4));

	SPI1_ClockEnable();

	// Reset SPI1 for defaults
	RCC->APB2RSTR |=  RCC_APB2RSTR_SPI1RST;
	RCC->APB2RSTR &=  ~RCC_APB2RSTR_SPI1RST;

	SPI1->CR1 |= CPHA_0|CPOL_0|SPI_Master|SSM_Enable|SSI_Set|Fpclk_2|MSB_First|Frame_8bit;

	SPI_Enable(SPI1);

	return 0;
}





















