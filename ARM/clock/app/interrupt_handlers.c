#include "delay.h"

extern void SysTick_Handler(void);

void SysTick_Handler(void)
{
	IncrementTickCount();
}
