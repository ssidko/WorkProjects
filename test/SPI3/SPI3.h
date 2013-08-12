#ifndef _SPI3_H
#define _SPI3_H

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_spi.h"

#define SPI3_MISO_Pin			GPIO_Pin_11
#define SPI3_MISO_Port			GPIOC

#define SPI3_MOSI_Pin			GPIO_Pin_12
#define SPI3_MOSI_Port			GPIOC

#define SPI3_SCLK_Pin			GPIO_Pin_10
#define SPI3_SCLK_Port			GPIOC

#define SPI3_SS_Pin				GPIO_Pin_15
#define SPI3_SS_Port			GPIOA

void SPI3_Configure(void);
void SPI3_Initialize(void);

void SPI3_Testing(void);

#endif // _SPI3_H
