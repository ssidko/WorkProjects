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
	init_port.GPIO_Speed = GPIO_Speed_50MHz;
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
	init_port.GPIO_Speed = GPIO_Speed_50MHz;
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
	init_port.GPIO_Speed = GPIO_Speed_50MHz;
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
	Delay(1);
	LCD_LineRESET(DISABLE);
	Delay(10);
	LCD_LineRESET(ENABLE);
	Delay(50);
}

void LCD_ili9320_Initialize(void)
{
	unsigned short tmp = 0;
	LCD_LineConfig();

	LCD_LineCS(ENABLE);
	LCD_LineRS(ENABLE);
	LCD_LineRD(ENABLE);
	LCD_LineWR(ENABLE);
	LCD_LineRESET(ENABLE);

	LCD_ili9320_Reset();

	LCD_ili9320_WriteToReg(0x00e5,0x8000);
	LCD_ili9320_WriteToReg(0x0000,0x0001);
	LCD_ili9320_WriteToReg(0x0001,0x0100);
	Delay(1);

	/*
	LCD_ili9320_WriteToReg(0x0002,0x0700);
	LCD_ili9320_WriteToReg(0x0004,0x0000);
	LCD_ili9320_WriteToReg(0x0008,0x0207);
	LCD_ili9320_WriteToReg(0x0009,0x0000);
	LCD_ili9320_WriteToReg(0x000a,0x0000);
	LCD_ili9320_WriteToReg(0x000c,0x0001);
	LCD_ili9320_WriteToReg(0x000d,0x0000);
	LCD_ili9320_WriteToReg(0x000f,0x0000);
	LCD_ili9320_WriteToReg(0x0010,0x0000);
	LCD_ili9320_WriteToReg(0x0011,0x0000);
	LCD_ili9320_WriteToReg(0x0012,0x0000);
	LCD_ili9320_WriteToReg(0x0013,0x0000);
	Delay(1);
	/*
	LCD_ili9320_WriteToReg(0x0010,0x17b0);
	LCD_ili9320_WriteToReg(0x0011,0x0137);
	Delay(1);
	LCD_ili9320_WriteToReg(0x0012,0x0139);
	Delay(1);
	LCD_ili9320_WriteToReg(0x0013,0x1700);
	LCD_ili9320_WriteToReg(0x0029,0x000c);
	Delay(1);
	/*
	LCD_ili9320_WriteToReg(0x0020,0x0000);
	LCD_ili9320_WriteToReg(0x0021,0x0000);


	/*
	LCD_ili9320_WriteToReg(0x0030,0x0000);
	LCD_ili9320_WriteToReg(0x0031,0x0507);
	LCD_ili9320_WriteToReg(0x0032,0x0104);
	LCD_ili9320_WriteToReg(0x0035,0x0105);
	LCD_ili9320_WriteToReg(0x0036,0x0404);
	LCD_ili9320_WriteToReg(0x0037,0x0603);
	LCD_ili9320_WriteToReg(0x0038,0x0004);
	LCD_ili9320_WriteToReg(0x0039,0x0007);
	LCD_ili9320_WriteToReg(0x003c,0x0501);
	LCD_ili9320_WriteToReg(0x003d,0x0404);
	LCD_ili9320_WriteToReg(0x0050,0x0000);
	LCD_ili9320_WriteToReg(0x0051,0x00ef);
	LCD_ili9320_WriteToReg(0x0052,0x0000);
	LCD_ili9320_WriteToReg(0x0053,0x013f);

	LCD_ili9320_WriteToReg(0x0060,0x2700);
	LCD_ili9320_WriteToReg(0x0061,0x0001);
	LCD_ili9320_WriteToReg(0x006a,0x0000);
	LCD_ili9320_WriteToReg(0x0080,0x0000);
	LCD_ili9320_WriteToReg(0x0081,0x0000);
	LCD_ili9320_WriteToReg(0x0082,0x0000);
	LCD_ili9320_WriteToReg(0x0083,0x0000);
	LCD_ili9320_WriteToReg(0x0084,0x0000);
	LCD_ili9320_WriteToReg(0x0085,0x0000);
	LCD_ili9320_WriteToReg(0x0090,0x0010);
	LCD_ili9320_WriteToReg(0x0092,0x0000);
	LCD_ili9320_WriteToReg(0x0093,0x0003);
	LCD_ili9320_WriteToReg(0x0095,0x0110);
	LCD_ili9320_WriteToReg(0x0097,0x0000);
	LCD_ili9320_WriteToReg(0x0098,0x0000);
	LCD_ili9320_WriteToReg(0x0007,0x0173);
	*/


	tmp = LCD_ili9320_ReadFromReg(0x0000);
	tmp = LCD_ili9320_ReadFromReg(0x0000);

	LCD_ili9320_WriteToReg(0x0022, 0x3333);
	LCD_ili9320_WriteToReg(0x0022, 0x3333);
	LCD_ili9320_WriteToReg(0x0022, 0x3333);
	LCD_ili9320_WriteToReg(0x0022, 0x3333);

	int x = 0;
	x++;
}

void LCD_ili9320_WriteToReg(unsigned short reg_index, unsigned short value)
{
	LCD_LineCS(DISABLE);
	LCD_LineRS(DISABLE);
	LCD_WriteToDB(reg_index);
	LCD_LineWR(DISABLE);
	LCD_LineWR(DISABLE);
	//Delay(1);
	LCD_LineWR(ENABLE);
	LCD_LineRS(ENABLE);
	//Delay(1);
	LCD_WriteToDB(value);
	LCD_LineWR(DISABLE);
	LCD_LineWR(DISABLE);
	//Delay(1);
	LCD_LineWR(ENABLE);
	LCD_LineCS(ENABLE);
}

unsigned short LCD_ili9320_ReadFromReg(unsigned short reg_index)
{
	unsigned short tmp = 0;

	LCD_LineCS(DISABLE);
	LCD_LineRS(DISABLE);
	LCD_WriteToDB(reg_index);
	LCD_LineWR(DISABLE);
	LCD_LineWR(DISABLE);
	LCD_LineWR(ENABLE);
	LCD_LineRS(ENABLE);

	LCD_ConfigureDBusIN();

	// Make Read strob
	LCD_LineRD(DISABLE);
	LCD_LineRD(DISABLE);
	LCD_LineRD(ENABLE);

	// Make Read strob
	//LCD_LineRD(DISABLE);
	//LCD_LineRD(DISABLE);
	//LCD_LineRD(ENABLE);

	tmp = LCD_ReadFromDB();

	LCD_LineCS(ENABLE);

	LCD_ConfigureDBusOUT();

	return tmp;
}













































