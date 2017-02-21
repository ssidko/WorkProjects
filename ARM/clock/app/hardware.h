#ifndef _HARDWARE_H
#define _HARDWARE_H

#include "stm32f1xx.h"
#include "gpio.h"
#include "spi.h"
#include "ssd1306.h"
#include "bmp280.h"

#define BMP280_CS_PORT							GPIOA
#define BMP280_CS_PIN							Pin4

#define LCD_WIDTH								128
#define LCD_HEIGHT								32

#define LED_PORT								GPIOC
#define LED_PIN									Pin13

#define LED_ON()								GPIO_PinSet(LED_PORT, LED_PIN)
#define LED_OFF()								GPIO_PinReset(LED_PORT, LED_PIN)
#define LED_TOGLE()								GPIO_ToglePin(LED_PORT, LED_PIN)

void HW_Init();
int SPI1_Init(void);

#endif // _HARDWARE_H
