#ifndef _HARDWARE_H
#define _HARDWARE_H

#include "stm32f1xx.h"
#include "gpio.h"
#include "spi.h"
#include "ssd1306.h"

#define LED_PORT								GPIOC
#define LED_PIN									Pin12

#define LCD_WIDTH								128
#define LCD_HEIGHT								32

void HW_Init();

#endif // _HARDWARE_H
