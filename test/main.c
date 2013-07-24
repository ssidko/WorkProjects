#include "stdint.h"
#include "system_stm32f4xx.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "core_cm4.h"

#define RCC_PLLCFGR_M						((uint32_t)8)
#define RCC_PLLCFGR_N						((uint32_t)336)
#define RCC_PLLCFGR_P						((uint32_t)2)
#define RCC_PLLCFGR_Q						((uint32_t)7)

inline void Delay(int delay)
{
	delay *= 1000;
	while (delay--);
}

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

void InitializeUSART2(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef init_port;
	init_port.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
	init_port.GPIO_Mode = GPIO_Mode_AF;
	init_port.GPIO_Speed = GPIO_Speed_50MHz;
	init_port.GPIO_OType = GPIO_OType_PP;
	init_port.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &init_port);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);

	USART_InitTypeDef usart_init;
	usart_init.USART_BaudRate = 9600;
	usart_init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	usart_init.USART_WordLength = USART_WordLength_8b;
	usart_init.USART_Parity = USART_Parity_No;
	usart_init.USART_StopBits = USART_StopBits_1;
	usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART2, &usart_init);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	NVIC_EnableIRQ(USART2_IRQn);

	USART_Cmd(USART2, ENABLE);

}

void USARTx_OutString(USART_TypeDef *USARTx, char *str)
{
	while (*str != 0x00) {
		while( !(USARTx->SR & USART_SR_TC) );
		USART_SendData(USARTx, (uint16_t)(*str));
		++str;
	}
}

int main(void)
{
	SystemInitialization();

	InitializeUSART2();

	USARTx_OutString(USART2, "System initialized\r\n");

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitTypeDef init_port_D;
	GPIO_StructInit(&init_port_D);
	init_port_D.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	init_port_D.GPIO_Mode = GPIO_Mode_OUT;
	init_port_D.GPIO_Speed = GPIO_Speed_2MHz;
	init_port_D.GPIO_OType = GPIO_OType_PP;
	init_port_D.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &init_port_D);


	GPIO_SetBits(GPIOD, GPIO_Pin_12);

	int res = SysTick_Config(16777214);

    while(1)
    {
    	GPIO_SetBits(GPIOD, GPIO_Pin_13);
    	Delay(1500);
    	GPIO_ResetBits(GPIOD, GPIO_Pin_13);
    	Delay(1000);
    }
}



void USART2_IRQHandler(void)
{
	int x = 0;
	x++;
	GPIO_SetBits(GPIOD, GPIO_Pin_15);
	Delay(1500);
	GPIO_ResetBits(GPIOD, GPIO_Pin_15);
	Delay(1000);
}

void SysTick_Handler(void)
{
	int x = 0;
	x++;
	GPIO_SetBits(GPIOD, GPIO_Pin_14);
	Delay(100);
	GPIO_ResetBits(GPIOD, GPIO_Pin_14);
	Delay(100);
}
