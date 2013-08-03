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

void LCD_ConfigureDBusOUT(void)
{
	GPIO_InitTypeDef init_port;
	init_port.GPIO_Pin = GPIO_Pin_All && ~GPIO_Pin_15;
	init_port.GPIO_Mode = GPIO_Mode_OUT;
	init_port.GPIO_Speed = GPIO_Speed_100MHz;
	init_port.GPIO_OType = GPIO_OType_PP;
	init_port.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &init_port);

	// LCD_DB1
	init_port.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOC, &init_port);
}

void LCD_ConfigureDBusIN(void)
{
	GPIO_InitTypeDef init_port;
	init_port.GPIO_Pin = GPIO_Pin_All & ~GPIO_Pin_15;
	init_port.GPIO_Mode = GPIO_Mode_IN;
	init_port.GPIO_Speed = GPIO_Speed_100MHz;
	init_port.GPIO_OType = GPIO_OType_PP;
	init_port.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &init_port);

	// LCD_DB1
	init_port.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOC, &init_port);
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

unsigned short LCD_ReadFromDB(void)
{
	unsigned short tmp = 0;

	tmp |= (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0) << 0);		// LCD_DB0
	tmp |= (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) << 1);	// LCD_DB1
	tmp |= (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2) << 2);		// LCD_DB2
	tmp |= (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) << 3);		// LCD_DB3
	tmp |= (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4) << 4);		// LCD_DB4
	tmp |= (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3) << 5);		// LCD_DB5
	tmp |= (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_6) << 6);		// LCD_DB6
	tmp |= (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_1) << 7);		// LCD_DB7
	tmp |= (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_8) << 8);		// LCD_DB8
	tmp |= (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_7) << 9);		// LCD_DB9
	tmp |= (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_10) << 10);	// LCD_DB10
	tmp |= (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_9) << 11);	// LCD_DB11
	tmp |= (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_12) << 12);	// LCD_DB12
	tmp |= (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_11) << 13);	// LCD_DB13
	tmp |= (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_14) << 14);	// LCD_DB14
	tmp |= (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_13) << 15);	// LCD_DB15

	return tmp;
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

void LCD_ili9320_Reset(void)
{
	LCD_LineRESET(ENABLE);
	Delay(20);
	LCD_LineRESET(DISABLE);
	Delay(20);
	LCD_LineRESET(ENABLE);
	Delay(100);
}

void LCD_ili9320_Initialize(void)
{
	LCD_LineConfig();

	LCD_LineCS(ENABLE);
	LCD_LineRS(ENABLE);
	LCD_LineRD(ENABLE);
	LCD_LineWR(ENABLE);
	LCD_LineRESET(ENABLE);

	LCD_ili9320_Reset();

	unsigned short tmp = 0;
	//LCD_ili9320_WriteToReg(0x3456, 0x3456);
	LCD_WriteToDB(0x3456);
	tmp = LCD_ReadFromDB();

	LCD_ili9320_WriteToReg(0x0000, 0x0001);
	LCD_ili9320_WriteToReg(0x0022, 0x3333);
	LCD_ili9320_WriteToReg(0x0022, 0x3333);
	LCD_ili9320_WriteToReg(0x0022, 0x3333);
	LCD_ili9320_WriteToReg(0x0022, 0x3333);
	tmp = LCD_ili9320_ReadFromReg(0x0000);
	tmp = LCD_ili9320_ReadFromReg(0x0000);
	tmp = LCD_ili9320_ReadFromReg(0x0000);
}

void LCD_ili9320_WriteToReg(unsigned short reg_index, unsigned short value)
{
	LCD_LineCS(DISABLE);
	LCD_LineRS(DISABLE);

	LCD_WriteToDB(reg_index);

	// Make Write strob
	LCD_LineWR(DISABLE);
	Delay(20);
	LCD_LineWR(ENABLE);

	LCD_LineRS(ENABLE);

	// Wait for processin command
	Delay(50);

	LCD_WriteToDB(value);

	// Make Write strob
	LCD_LineWR(DISABLE);
	Delay(20);
	LCD_LineWR(ENABLE);

	// Wait for processin command
	Delay(50);

	LCD_LineCS(ENABLE);
}

unsigned short LCD_ili9320_ReadFromReg(unsigned short reg_index)
{
	unsigned short tmp = 0;

	LCD_LineCS(DISABLE);
	LCD_LineRS(DISABLE);

	LCD_WriteToDB(reg_index);

	// Make Write strob
	LCD_LineWR(DISABLE);
	Delay(20);
	LCD_LineWR(ENABLE);

	LCD_LineRS(ENABLE);

	// Wait for processing command
	Delay(50);

	LCD_ConfigureDBusIN();

	// Make Read strob
	LCD_LineRD(DISABLE);
	Delay(20);
	LCD_LineRD(ENABLE);

	// Wait for processin command
	Delay(50);

	tmp = LCD_ReadFromDB();

	LCD_LineCS(ENABLE);

	LCD_ConfigureDBusOUT();

	return tmp;
}













































