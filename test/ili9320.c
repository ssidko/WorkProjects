#include "ili9320.h"

#define LCD_CS_PIN						GPIO_Pin_11
#define LCD_CS_PORT						GPIOB

#define LCD_RS_PIN						GPIO_Pin_15
#define LCD_RS_PORT						GPIOE

#define LCD_RD_PIN						GPIO_Pin_13
#define LCD_RD_PORT						GPIOB

#define LCD_WR_PIN						GPIO_Pin_15
#define LCD_WR_PORT						GPIOB

#define LCD_RESET_PIN					GPIO_Pin_9
#define LCD_RESET_PORT					GPIOD

void LCD_LineConfig(void)
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
}

void LCD_WriteToDB(unsigned short val)
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

void LCD_LineCS(FunctionalState new_state)
{
	(new_state == ENABLE) ? GPIO_SetBits(LCD_CS_PORT, LCD_CS_PIN) : GPIO_ResetBits(LCD_CS_PORT, LCD_CS_PIN);
}

void LCD_LineRS(FunctionalState new_state)
{
	(new_state == ENABLE) ? GPIO_SetBits(LCD_RS_PORT, LCD_RS_PIN) : GPIO_ResetBits(LCD_RS_PORT, LCD_RS_PIN);
}

void LCD_LineRD(FunctionalState new_state)
{
	(new_state == ENABLE) ? GPIO_SetBits(LCD_RD_PORT, LCD_RD_PIN) : GPIO_ResetBits(LCD_RD_PORT, LCD_RD_PIN);
}

void LCD_LineWR(FunctionalState new_state)
{
	(new_state == ENABLE) ? GPIO_SetBits(LCD_WR_PORT, LCD_WR_PIN) : GPIO_ResetBits(LCD_WR_PORT, LCD_WR_PIN);
}

void LCD_LineRESET(FunctionalState new_state)
{
	(new_state == ENABLE) ? GPIO_SetBits(LCD_RESET_PORT, LCD_RESET_PIN) : GPIO_ResetBits(LCD_RESET_PORT, LCD_RESET_PIN);
}









