#include "stdint.h"
#include "system_stm32f4xx.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "core_cm4.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_exti.h"
#include "misc.h"
#include "stm32f4xx_tim.h"
#include "ili9320.h"

#define RCC_PLLCFGR_M						((uint32_t)8)
#define RCC_PLLCFGR_N						((uint32_t)336)
#define RCC_PLLCFGR_P						((uint32_t)0) // PLLP = 2
#define RCC_PLLCFGR_Q						((uint32_t)7)

int sys_tick_counter;

inline void Delay(int delay)
{
	sys_tick_counter = delay;
	while (sys_tick_counter);
}

void USARTx_OutString(USART_TypeDef *USARTx, char *str)
{
	while (*str != 0x00) {
		while((USARTx->SR & USART_SR_TXE) == RESET);
		USART_SendData(USARTx, (uint16_t)(*str));
		++str;
	}
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
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;

	// В качестве источника частоты system clock (SYSCLK) выбираем PLL (168 MHz).
	RCC->CFGR |= RCC_CFGR_SW_PLL;
}

void InitializeUSART2(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef init_port;
	init_port.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	init_port.GPIO_Mode = GPIO_Mode_AF;
	init_port.GPIO_Speed = GPIO_Speed_50MHz;
	init_port.GPIO_OType = GPIO_OType_PP;
	init_port.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &init_port);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

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

void InitializeTIM4(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	GPIO_InitTypeDef init_port;
	init_port.GPIO_Pin = GPIO_Pin_15;
	init_port.GPIO_Mode = GPIO_Mode_AF;
	init_port.GPIO_Speed = GPIO_Speed_50MHz;
	init_port.GPIO_OType = GPIO_OType_PP;
	init_port.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &init_port);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);

	TIM_TimeBaseInitTypeDef tim_init;
	tim_init.TIM_Prescaler = 0;
	tim_init.TIM_CounterMode = TIM_CounterMode_Up;
	tim_init.TIM_Period = 1000; // TIMx_ARR
	tim_init.TIM_ClockDivision = TIM_CKD_DIV1;
	tim_init.TIM_RepetitionCounter - 0x00; // Ignored
	TIM_TimeBaseInit(TIM4, &tim_init);

	TIM_OCInitTypeDef oc_tim_init;
	oc_tim_init.TIM_OCMode = TIM_OCMode_PWM2/*TIM_OCMode_PWM2*/;
	oc_tim_init.TIM_OutputState = TIM_OutputState_Enable/*TIM_OutputState_Disable*/;
	oc_tim_init.TIM_OutputNState = 0x00; // Ignored. This parameter is valid only for TIM1 and TIM8. */
	oc_tim_init.TIM_Pulse = 10;
	oc_tim_init.TIM_OCPolarity = /*TIM_OCPolarity_High*/TIM_OCPolarity_Low;
	oc_tim_init.TIM_OCNPolarity = 0x00; // Ignored. This parameter is valid only for TIM1 and TIM8. */
	oc_tim_init.TIM_OCIdleState = 0x00; // Ignored. This parameter is valid only for TIM1 and TIM8. */
	oc_tim_init.TIM_OCNIdleState = 0x00; // Ignored. This parameter is valid only for TIM1 and TIM8. */
	TIM_OC4Init(TIM4, &oc_tim_init);

	  /* TIM1 counter enable */
	TIM_Cmd(TIM4, ENABLE);

	  /* TIM1 Main Output Enable */
	TIM_CtrlPWMOutputs(TIM4, ENABLE);

}

void InitializeUserButton(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef init_port;
	init_port.GPIO_Pin = GPIO_Pin_0;
	init_port.GPIO_Mode = GPIO_Mode_IN;
	init_port.GPIO_Speed = GPIO_Speed_2MHz;
	init_port.GPIO_OType = GPIO_OType_PP;
	init_port.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &init_port);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

	EXTI_InitTypeDef exti_init;
	exti_init.EXTI_Line = EXTI_Line0;
	exti_init.EXTI_Mode = EXTI_Mode_Interrupt;
	exti_init.EXTI_Trigger = EXTI_Trigger_Rising;
	exti_init.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti_init);

	NVIC_EnableIRQ(EXTI0_IRQn);
}



#define USART_RX_BUFFER_LEN			128

static char usart_rx_buffer[USART_RX_BUFFER_LEN] = {0};
static char *rx_buff_ptr = usart_rx_buffer;

int led;

void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
		led = !(led);
	    /* Clear the EXTI line 0 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

int main(void)
{
	SystemInitialization();

	InitializeUSART2();

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitTypeDef init_port_D;
	GPIO_StructInit(&init_port_D);
	init_port_D.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	init_port_D.GPIO_Mode = GPIO_Mode_OUT;
	init_port_D.GPIO_Speed = GPIO_Speed_2MHz;
	init_port_D.GPIO_OType = GPIO_OType_PP;
	init_port_D.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &init_port_D);

	GPIO_SetBits(GPIOD, GPIO_Pin_12);

	SysTick_Config(168000);

	USARTx_OutString(USART2, "System initialized.\r\n");

	InitializeUserButton();

	//
	// Work with LCD
	//
	LCD_LineConfig();



	InitializeTIM4();

	int pulse = 0;
	int is_fading = 0;
	int step = 4;
	int delay = 5;
	int pulse_threshold = 1000;

    while(1)
    {
    	/*
    	GPIO_SetBits(GPIOD, GPIO_Pin_13);
    	Delay(1000);
    	GPIO_ResetBits(GPIOD, GPIO_Pin_13);
    	Delay(1000);
    	if (led) {
        	GPIO_SetBits(GPIOD, GPIO_Pin_14);
        	Delay(200);
        	GPIO_ResetBits(GPIOD, GPIO_Pin_14);
        	Delay(200);
    	}
    	*/
    	/*
    	pulse = 0;
    	while(pulse < pulse_threshold) {
    		pulse += step;
    		TIM4->CCR4 = pulse;
    		Delay(delay);
    	}
    	*/

    	pulse = 1000;
    	while(pulse > 0) {
    		pulse -= step;
    		TIM4->CCR4 = pulse;
    		Delay(delay);
    	}
    	Delay(200);
    }
}

void USART2_IRQHandler(void)
{
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
		*rx_buff_ptr = USART_ReceiveData(USART2);
		++rx_buff_ptr;
	}
}

void SysTick_Handler(void)
{
	--sys_tick_counter;
}
