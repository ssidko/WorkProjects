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
    FLASH->ACR = FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;

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

//Format Conversion Logic
#define RGB565(RED,GREEN,BLUE)  ((((RED&0x03F)>>1)<<11)|((GREEN&0x03F)<<5)|((BLUE&0x03F)>>1))

void InitializeLCD(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);


	GPIO_InitTypeDef init_port;
	init_port.GPIO_Pin = GPIO_Pin_All;
	init_port.GPIO_Mode = GPIO_Mode_OUT;
	init_port.GPIO_Speed = GPIO_Speed_100MHz;
	init_port.GPIO_OType = GPIO_OType_PP;
	init_port.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &init_port);

	// LCD_DB1
	init_port.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOC, &init_port);

	// LCD_CS, LCD_RD, LCD_WR.
	init_port.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_Init(GPIOB, &init_port);

	// LCD_RESET
	init_port.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOD, &init_port);

	// Set LCD_RESET line to high "1".
	GPIO_SetBits(GPIOD, GPIO_Pin_9);

	// Set LCD_CS to "1"
	GPIO_SetBits(GPIOB, GPIO_Pin_11);

	// Set LCD_RD to "1"
	GPIO_SetBits(GPIOB, GPIO_Pin_13);

	// Set LCD_WR to "1"
	GPIO_SetBits(GPIOB, GPIO_Pin_15);

}

void LCD_Reset(void)
{
	//Make Reset
	GPIO_SetBits(GPIOD, GPIO_Pin_9);
	Delay(50);
	GPIO_ResetBits(GPIOD, GPIO_Pin_9);
	Delay(50);
	GPIO_SetBits(GPIOD, GPIO_Pin_9);
	Delay(50);
}

void LCD_WriteToBus(unsigned short val)
{
	((val >> 0) & 0x0001) ? GPIO_SetBits(GPIOE, GPIO_Pin_0) : GPIO_ResetBits(GPIOE, GPIO_Pin_0);	// LCD_DB0
	((val >> 1) & 0x0001) ? GPIO_SetBits(GPIOC, GPIO_Pin_13) : GPIO_ResetBits(GPIOC, GPIO_Pin_13);	// LCD_DB1
	((val >> 2) & 0x0001) ? GPIO_SetBits(GPIOE, GPIO_Pin_2) : GPIO_ResetBits(GPIOE, GPIO_Pin_2);	// LCD_DB2
	((val >> 3) & 0x0001) ? GPIO_SetBits(GPIOE, GPIO_Pin_5) : GPIO_ResetBits(GPIOE, GPIO_Pin_5);	// LCD_DB3
	((val >> 4) & 0x0001) ? GPIO_SetBits(GPIOE, GPIO_Pin_4) : GPIO_ResetBits(GPIOE, GPIO_Pin_4);	// LCD_DB4
	((val >> 5) & 0x0001) ? GPIO_SetBits(GPIOE, GPIO_Pin_3) : GPIO_ResetBits(GPIOE, GPIO_Pin_3);	// LCD_DB5
	((val >> 6) & 0x0001) ? GPIO_SetBits(GPIOE, GPIO_Pin_6) : GPIO_ResetBits(GPIOE, GPIO_Pin_6);	// LCD_DB6
	((val >> 7) & 0x0001) ? GPIO_SetBits(GPIOE, GPIO_Pin_1) : GPIO_ResetBits(GPIOE, GPIO_Pin_1);	// LCD_DB7
	((val >> 8) & 0x0001) ? GPIO_SetBits(GPIOE, GPIO_Pin_8) : GPIO_ResetBits(GPIOE, GPIO_Pin_8);	// LCD_DB8
	((val >> 9) & 0x0001) ? GPIO_SetBits(GPIOE, GPIO_Pin_7) : GPIO_ResetBits(GPIOE, GPIO_Pin_7);	// LCD_DB9
	((val >> 10) & 0x0001) ? GPIO_SetBits(GPIOE, GPIO_Pin_10) : GPIO_ResetBits(GPIOE, GPIO_Pin_10);	// LCD_DB10
	((val >> 11) & 0x0001) ? GPIO_SetBits(GPIOE, GPIO_Pin_9) : GPIO_ResetBits(GPIOE, GPIO_Pin_9);	// LCD_DB11
	((val >> 12) & 0x0001) ? GPIO_SetBits(GPIOE, GPIO_Pin_12) : GPIO_ResetBits(GPIOE, GPIO_Pin_12);	// LCD_DB12
	((val >> 13) & 0x0001) ? GPIO_SetBits(GPIOE, GPIO_Pin_11) : GPIO_ResetBits(GPIOE, GPIO_Pin_11);	// LCD_DB13
	((val >> 14) & 0x0001) ? GPIO_SetBits(GPIOE, GPIO_Pin_14) : GPIO_ResetBits(GPIOE, GPIO_Pin_14);	// LCD_DB14
	((val >> 15) & 0x0001) ? GPIO_SetBits(GPIOE, GPIO_Pin_13) : GPIO_ResetBits(GPIOE, GPIO_Pin_13);	// LCD_DB15
}

void main_W_com_data(unsigned short com, unsigned short data)
{
	// Set LCD_CS to "0"
	GPIO_ResetBits(GPIOB, GPIO_Pin_11);
	//Delay(5);

	// Set LCD_RS to "0"
	GPIO_ResetBits(GPIOE, GPIO_Pin_15);
	//Delay(10);

	LCD_WriteToBus(com);

	GPIO_ResetBits(GPIOB, GPIO_Pin_15);	// Set LCD_WR to "0"
	Delay(10);
	GPIO_SetBits(GPIOB, GPIO_Pin_15); 	// Set LCD_WR to "1"
	Delay(10);

	// Set LCD_RS to "1"
	GPIO_SetBits(GPIOE, GPIO_Pin_15);
	//Delay(100);

	LCD_WriteToBus(data);

	GPIO_ResetBits(GPIOB, GPIO_Pin_15);	// Set LCD_WR to "0"
	Delay(10);
	GPIO_SetBits(GPIOB, GPIO_Pin_15); 	// Set LCD_WR to "1"
	Delay(10);

	// Set LCD_CS to "1"
	GPIO_SetBits(GPIOB, GPIO_Pin_11);
}

void InitializeEx(void)
{
	  //  VENDOR
	  main_W_com_data(0x0046,0x00A4);
	  main_W_com_data(0x0047,0x0053);
	  main_W_com_data(0x0048,0x0000);
	  main_W_com_data(0x0049,0x0044);
	  main_W_com_data(0x004a,0x0004);
	  main_W_com_data(0x004b,0x0067);
	  main_W_com_data(0x004c,0x0033);
	  main_W_com_data(0x004d,0x0077);
	  main_W_com_data(0x004e,0x0012);
	  main_W_com_data(0x004f,0x004C);
	  main_W_com_data(0x0050,0x0046);
	  main_W_com_data(0x0051,0x0044);

	  //240x320 window setting
	  main_W_com_data(0x0002,0x0000); // Column address start2
	  main_W_com_data(0x0003,0x0000); // Column address start1
	  main_W_com_data(0x0004,0x0000); // Column address end2
	  main_W_com_data(0x0005,0x00ef); // Column address end1
	  main_W_com_data(0x0006,0x0000); // Row address start2
	  main_W_com_data(0x0007,0x0000); // Row address start1
	  main_W_com_data(0x0008,0x0001); // Row address end2
	  main_W_com_data(0x0009,0x003f); // Row address end1

	  // Display Setting
	  main_W_com_data(0x0001,0x0006); // IDMON=0, INVON=1, NORON=1, PTLON=0
	  main_W_com_data(0x0016,0x00C8); // MY=0, MX=0, MV=0, ML=1, BGR=0, TEON=0   0048
	  main_W_com_data(0x0023,0x0095); // N_DC=1001 0101
	  main_W_com_data(0x0024,0x0095); // PI_DC=1001 0101
	  main_W_com_data(0x0025,0x00FF); // I_DC=1111 1111

	  main_W_com_data(0x0027,0x0002); // N_BP=0000 0010
	  main_W_com_data(0x0028,0x0002); // N_FP=0000 0010
	  main_W_com_data(0x0029,0x0002); // PI_BP=0000 0010
	  main_W_com_data(0x002a,0x0002); // PI_FP=0000 0010
	  main_W_com_data(0x002C,0x0002); // I_BP=0000 0010
	  main_W_com_data(0x002d,0x0002); // I_FP=0000 0010

	  main_W_com_data(0x003a,0x0001); // N_RTN=0000, N_NW=001    0001
	  main_W_com_data(0x003b,0x0000); // P_RTN=0000, P_NW=001
	  main_W_com_data(0x003c,0x00f0); // I_RTN=1111, I_NW=000
	  main_W_com_data(0x003d,0x0000); // DIV=00
	  Delay(5);
	  main_W_com_data(0x0035,0x0038); // EQS=38h
	  main_W_com_data(0x0036,0x0078); // EQP=78h
	  main_W_com_data(0x003E,0x0038); // SON=38h
	  main_W_com_data(0x0040,0x000F); // GDON=0Fh
	  main_W_com_data(0x0041,0x00F0); // GDOFF

	  // Power Supply Setting
	  main_W_com_data(0x0019,0x0049); // CADJ=0100, CUADJ=100, OSD_EN=1 ,60Hz
	  main_W_com_data(0x0093,0x000F); // RADJ=1111, 100%
	  Delay(5);
	  main_W_com_data(0x0020,0x0040); // BT=0100
	  main_W_com_data(0x001D,0x0007); // VC1=111   0007
	  main_W_com_data(0x001E,0x0000); // VC3=000
	  main_W_com_data(0x001F,0x0004); // VRH=0011

	  //VCOM SETTING
	  main_W_com_data(0x0044,0x004D); // VCM=101 0000  4D
	  main_W_com_data(0x0045,0x000E); // VDV=1 0001   0011
	  Delay(5);
	  main_W_com_data(0x001C,0x0004); // AP=100
	  Delay(5);

	  main_W_com_data(0x001B,0x0018); // GASENB=0, PON=0, DK=1, XDK=0, VLCD_TRI=0, STB=0
	  Delay(5);
	  main_W_com_data(0x001B,0x0010); // GASENB=0, PON=1, DK=0, XDK=0, VLCD_TRI=0, STB=0
	  Delay(5);
	  main_W_com_data(0x0043,0x0080); //set VCOMG=1
	  Delay(5);

	  // Display ON Setting
	  main_W_com_data(0x0090,0x007F); // SAP=0111 1111
	  main_W_com_data(0x0026,0x003C); //GON=0, DTE=0, D=01
	  Delay(5);
	  main_W_com_data(0x0026,0x0024); //GON=1, DTE=0, D=01
	  main_W_com_data(0x0026,0x002C); //GON=1, DTE=0, D=11
	  Delay(5);
	  main_W_com_data(0x0026,0x003C); //GON=1, DTE=1, D=11

	  // INTERNAL REGISTER SETTING
	  main_W_com_data(0x0057,0x0002); // TEST_Mode=1: into TEST mode
	  main_W_com_data(0x0095,0x0001); // SET DISPLAY CLOCK AND PUMPING CLOCK TO SYNCHRONIZE
	  main_W_com_data(0x0057,0x0000); // TEST_Mode=0: exit TEST mode

	  main_W_com_data(0x0022,0x0000);
}

void LCD_WriteToGRAM(unsigned short val)
{
	// Set LCD_RD to "1"
	GPIO_SetBits(GPIOB, GPIO_Pin_13);
	Delay(100);

	// Set LCD_CS to "0"
	GPIO_ResetBits(GPIOB, GPIO_Pin_11);
	Delay(100);

	// Set LCD_RS to "0"
	GPIO_ResetBits(GPIOE, GPIO_Pin_15);
	Delay(100);

	// Write command 0x22
	LCD_WriteToBus(0x22);
	Delay(100);
	GPIO_ResetBits(GPIOB, GPIO_Pin_15);	// Set LCD_WR to "0"
	Delay(200);
	GPIO_SetBits(GPIOB, GPIO_Pin_15); 	// Set LCD_WR to "1"
	Delay(500);

	// Set LCD_RS to "1"
	GPIO_SetBits(GPIOE, GPIO_Pin_15);
	Delay(100);

	// Write in GRAM
	LCD_WriteToBus(val);
	GPIO_ResetBits(GPIOB, GPIO_Pin_15);	// Set LCD_WR to "0"
	Delay(200);
	GPIO_SetBits(GPIOB, GPIO_Pin_15); 	// Set LCD_WR to "1"
	Delay(500);

	// Set LCD_CS to "1"
	GPIO_SetBits(GPIOB, GPIO_Pin_11);
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
	LCD_Reset();
	InitializeLCD();
	InitializeEx();
	main_W_com_data(0x22, 0x4444);
	main_W_com_data(0x22, 0x4444);
	main_W_com_data(0x22, 0x4444);
	main_W_com_data(0x22, 0x4444);
	main_W_com_data(0x22, 0x4444);
	LCD_WriteToGRAM(0x4444);

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
