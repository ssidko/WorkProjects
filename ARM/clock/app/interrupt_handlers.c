#include "delay.h"

extern void SysTick_Handler(void);

void SysTick_Handler(void)
{
	IncrementTickCount();
}

extern RTC_Handler(void);

void RTC_IRQHandler(void)
{
	static int x = 0;
	x++;
	RTC_Handler();
}
