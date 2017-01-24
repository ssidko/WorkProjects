#include "delay.h"

volatile static uint32_t tick_counter = 0;

void IncrementTickCount()
{
	++tick_counter;
}

uint32_t GetTickCount(void)
{
	return tick_counter;
}

void Delay_ms(uint32_t ms)
{
	uint32_t start = GetTickCount();
	while (GetTickCount() - start < ms) {}
}
