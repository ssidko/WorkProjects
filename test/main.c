#include "stdint.h"
#include "system_stm32f4xx.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#define VECT_TAB_OFFSET  0x00

#define RCC_PLLCFGR_M						((uint32_t)0x00000008)
#define RCC_PLLCFGR_N						((uint32_t)0x00000150) // 336
#define RCC_PLLCFGR_P						((uint32_t)0x00000002)
#define RCC_PLLCFGR_Q						((uint32_t)0x00000007)

void SystemInitialization(void)
{
	// Подключаем HSE (8 MHz) и ожидаем его стабилизации.
	RCC->CR |= RCC_CR_HSEON;
	while ((RCC->CR & RCC_CR_HSERDY) == 0x00) {}

	// Reset RCC->CFGR
	RCC->CFGR = 0x00000000;

	// SYSCLK not divided. AHB = 168 MHz.
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

	// HCLK divided by 2. APB2 = 84 MHz
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

	// HCLK divided by 4. APB1 = 42 MHz
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

	// Select regulator voltage output Scale 1 mode, System frequency up to 168 MHz
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_VOS;

	// Конфигурирование PLL
	RCC->PLLCFGR = RCC_PLLCFGR_M | (RCC_PLLCFGR_N << 6) | (RCC_PLLCFGR_P << 16) | RCC_PLLCFGR_PLLSRC_HSE | (RCC_PLLCFGR_Q << 24) ;

	// Активируем PLL и ожидаем его готовности.
	RCC->CR |= RCC_CR_PLLON;
	while ((RCC->CR & RCC_CR_PLLRDY) == 0x00) {}

    /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
    FLASH->ACR = FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;

	// В качестве источника частоты system clock (SYSCLK) выбираем PLL (168 MHz).
	RCC->CFGR |= RCC_CFGR_SW_PLL;
}

int main(void)
{

	//SystemInit();

	SystemInitialization();

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
