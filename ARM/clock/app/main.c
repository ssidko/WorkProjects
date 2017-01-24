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
#define FRAME_BUFFER_SIZE				(128*32/8)
uint8_t frame_buffer[FRAME_BUFFER_SIZE];

int main(void)
{
	SystemCoreClockUpdate();

	HW_Init();

	for (uint32_t i = 0; i < FRAME_BUFFER_SIZE; i++) {
		frame_buffer[i] = 0x55;
	}

	ssd1306_UpdateScreen(frame_buffer, FRAME_BUFFER_SIZE);

    while(1)
    {
    	ssd1306_Sleep();
    	Delay_ms(5000);

    	GPIO_ToglePin(GPIOC, LED_PIN);
    	//Delay_ms(1000);

    	ssd1306_Wakeup();
    	//ssd1306_UpdateScreen(frame_buffer, FRAME_BUFFER_SIZE);
    	Delay_ms(2000);
    }
}













































