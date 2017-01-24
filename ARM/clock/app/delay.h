#ifndef _DELAY_H
#define _DELAY_H

#include <stdint.h>

void IncrementTickCount(void);
uint32_t GetTickCount(void);
void Delay_ms(uint32_t ms);

#endif // _DELAY_H
