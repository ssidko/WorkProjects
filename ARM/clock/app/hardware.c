#include "hardware.h"

void HW_Init()
{
	// Port-C clock ENABLE
	GPIOC_ClockEnable();

	// LED pin configure
	LED_PORT->CRH &= (0x0F << ((LED_PIN - 8) * 4));
	LED_PORT->CRH |= ((Out_2Mhz|Out_PushPull) << ((LED_PIN - 8) * 4));

	SSD1306_Init(128, 32);
}
