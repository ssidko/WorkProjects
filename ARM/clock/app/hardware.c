#include "hardware.h"

void HW_Init()
{
	// Port-C clock ENABLE
	GPIOC_ClockEnable();

	// LED pin configure
	// General purpose output Open-drain
	// Output mode, max speed 2 MHz
	GPIOC->CRH &= ~(GPIO_CRH_MODE12 | GPIO_CRH_CNF12);
	GPIOC->CRH |= (Out_2Mhz|Out_OpenDrain) << GPIO_CRH_MODE12_Pos;

	SSD1306_Init(128, 32);
}
