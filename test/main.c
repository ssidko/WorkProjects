#include "stdint.h"
#include "system_stm32f4xx.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

void SystemInitialization(void)
{
	// Подключаем HSE (8 MHz).
	RCC->CR |= RCC_CR_HSEON;

	// Ожидаем пока стабилизируется HSE.
	while (RCC->CR & RCC_CR_HSERDY) {}

	// В качестве источника частоты system clock (SYSCLK) выбираем HSE (8 MHz).
	RCC->CFGR |= RCC_CFGR_SW_HSE;

}

int main(void)
{

	SystemInit();

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitTypeDef init_port_D;
	GPIO_StructInit(&init_port_D);

	init_port_D.GPIO_Pin = /*GPIO_Pin_All*/GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	init_port_D.GPIO_Mode = GPIO_Mode_OUT;
	init_port_D.GPIO_Speed = GPIO_Speed_50MHz;
	init_port_D.GPIO_OType = GPIO_OType_PP;
	init_port_D.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOD, &init_port_D);

	//GPIO_SetBits(GPIOD, GPIO_Pin_5);

	GPIO_SetBits(GPIOD, GPIO_Pin_12);
	GPIO_SetBits(GPIOD, GPIO_Pin_13);
	GPIO_SetBits(GPIOD, GPIO_Pin_14);
	GPIO_SetBits(GPIOD, GPIO_Pin_15);

	/*
	GPIO_ResetBits(GPIOD, GPIO_Pin_12);
	GPIO_ResetBits(GPIOD, GPIO_Pin_13);
	GPIO_ResetBits(GPIOD, GPIO_Pin_14);
	GPIO_ResetBits(GPIOD, GPIO_Pin_15);
	*/

    while(1)
    {
    	int x = 0;
    }
}
