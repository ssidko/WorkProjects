#include "ili9320.h"

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
	init_port.GPIO_Pin = GPIO_Pin_All & ~GPIO_Pin_15;
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
	LCD_SetRESET;
	Delay(1);
	LCD_ClearRESET;
	Delay(10);
	LCD_SetRESET;
	Delay(50);
}

void LCD_ili9320_Initialize(void)
{
	unsigned short tmp = 0;
	LCD_LineConfig();

	LCD_SetCS;
	LCD_SetRS;
	LCD_SetRD;
	LCD_SetWR;
	LCD_SetRESET;

	LCD_ili9320_Reset();

	//************* Start Initial Sequence **********//
	LCD_ili9320_WriteToReg(0x00E5, 0x8000); // Set the internal vcore voltage
	LCD_ili9320_WriteToReg(0x0000, 0x0001); // Start internal OSC.
	LCD_ili9320_WriteToReg(0x0001, 0x0100); // set SS and SM bit
	LCD_ili9320_WriteToReg(0x0002, 0x0700); // set 1 line inversion
	LCD_ili9320_WriteToReg(0x0003, 0x1030); // set GRAM write direction and BGR=1.
	LCD_ili9320_WriteToReg(0x0004, 0x0000); // Resize register
	LCD_ili9320_WriteToReg(0x0008, 0x0202); // set the back porch and front porch
	LCD_ili9320_WriteToReg(0x0009, 0x0000); // set non-display area refresh cycle ISC[3:0]
	LCD_ili9320_WriteToReg(0x000A, 0x0000); // FMARK function
	LCD_ili9320_WriteToReg(0x000C, 0x0000); // RGB interface setting
	LCD_ili9320_WriteToReg(0x000D, 0x0000); // Frame marker Position
	LCD_ili9320_WriteToReg(0x000F, 0x0000); // RGB interface polarity
	//*************Power On sequence ****************//
	LCD_ili9320_WriteToReg(0x0010, 0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB
	LCD_ili9320_WriteToReg(0x0011, 0x0000); // DC1[2:0], DC0[2:0], VC[2:0]
	LCD_ili9320_WriteToReg(0x0012, 0x0000); // VREG1OUT voltage
	LCD_ili9320_WriteToReg(0x0013, 0x0000); // VDV[4:0] for VCOM amplitude
	Delay(200); // Dis-charge capacitor power voltage
	LCD_ili9320_WriteToReg(0x0010, 0x17B0); // SAP, BT[3:0], AP, DSTB, SLP, STB
	LCD_ili9320_WriteToReg(0x0011, 0x0037); // DC1[2:0], DC0[2:0], VC[2:0]
	Delay(50); // Delay 50ms
	LCD_ili9320_WriteToReg(0x0012, 0x013B); // VREG1OUT voltage
	Delay(50); // Delay 50ms
	LCD_ili9320_WriteToReg(0x0013, 0x1800); // VDV[4:0] for VCOM amplitude
	LCD_ili9320_WriteToReg(0x0029, 0x000F); // VCM[4:0] for VCOMH
	Delay(50);
	LCD_ili9320_WriteToReg(0x0020, 0x0000); // GRAM horizontal Address
	LCD_ili9320_WriteToReg(0x0021, 0x0000); // GRAM Vertical Address
	// ----------- Adjust the Gamma Curve ----------//
	LCD_ili9320_WriteToReg(0x0030, 0x0000);
	LCD_ili9320_WriteToReg(0x0031, 0x0007);
	LCD_ili9320_WriteToReg(0x0032, 0x0103);
	LCD_ili9320_WriteToReg(0x0035, 0x0407);
	LCD_ili9320_WriteToReg(0x0036, 0x090F);
	LCD_ili9320_WriteToReg(0x0037, 0x0404);
	LCD_ili9320_WriteToReg(0x0038, 0x0400);
	LCD_ili9320_WriteToReg(0x0039, 0x0404);
	LCD_ili9320_WriteToReg(0x003C, 0x0000);
	LCD_ili9320_WriteToReg(0x003D, 0x0400);
	//------------------ Set GRAM area ---------------//
	LCD_ili9320_WriteToReg(0x0050, 0x0000); // Horizontal GRAM Start Address
	LCD_ili9320_WriteToReg(0x0051, 0x00EF); // Horizontal GRAM End Address
	LCD_ili9320_WriteToReg(0x0052, 0x0000); // Vertical GRAM Start Address
	LCD_ili9320_WriteToReg(0x0053, 0x013F); // Vertical GRAM Start Address
	LCD_ili9320_WriteToReg(0x0060, 0x2700); // Gate Scan Line
	LCD_ili9320_WriteToReg(0x0061, 0x0001); // NDL,VLE, REV
	LCD_ili9320_WriteToReg(0x006A, 0x0000); // set scrolling line
	//-------------- Partial Display Control ---------//
	LCD_ili9320_WriteToReg(0x0080, 0x0000);
	LCD_ili9320_WriteToReg(0x0081, 0x0000);
	LCD_ili9320_WriteToReg(0x0082, 0x0000);
	LCD_ili9320_WriteToReg(0x0083, 0x0000);
	LCD_ili9320_WriteToReg(0x0084, 0x0000);
	LCD_ili9320_WriteToReg(0x0085, 0x0000);
	//-------------- Panel Control -------------------//
	LCD_ili9320_WriteToReg(0x0090, 0x0010);
	LCD_ili9320_WriteToReg(0x0092, 0x0000);
	LCD_ili9320_WriteToReg(0x0093, 0x0003);
	LCD_ili9320_WriteToReg(0x0095, 0x0110);
	LCD_ili9320_WriteToReg(0x0097, 0x0000);
	LCD_ili9320_WriteToReg(0x0098, 0x0000);
	LCD_ili9320_WriteToReg(0x0007, 0x0033); // 262K color and display ON

	int i = 0;
	unsigned short colour = Magenta;
	/*
	LCD_StartWriteGRAM();
	for (; i < 320*240; ++i) {
		LCD_WriteToDB(colour);
		LCD_ClearWR;
		LCD_ClearWR;
		LCD_SetWR;
	}
	LCD_LineCS(ENABLE);
	*/
	int x = 0;
	x++;
}

void LCD_ili9320_WriteToReg(unsigned short reg_index, unsigned short value)
{
	LCD_ClearCS;
	Delay(1);
	LCD_ClearRS;
	Delay(1);
	LCD_WriteToDB(reg_index);
	LCD_ClearWR;
	Delay(1);
	LCD_SetWR;
	Delay(1);
	LCD_SetRS;
	Delay(1);
	LCD_WriteToDB(value);
	LCD_ClearWR;
	Delay(1);
	LCD_SetWR;
	Delay(1);
	LCD_SetCS;
}

unsigned short LCD_ili9320_ReadFromReg(unsigned short reg_index)
{
	unsigned short data = 0;

	LCD_ClearCS;
	LCD_ClearRS;
	LCD_WriteToDB(reg_index);
	LCD_ClearWR;
	LCD_ClearWR;
	LCD_SetWR;
	LCD_SetRS;

	LCD_ConfigureDBusIN();
	LCD_ClearRD;
	LCD_ClearRD;
	LCD_SetRD;
	data = LCD_ReadFromDB();
	LCD_SetCS;
	LCD_ConfigureDBusOUT();

	return data;
}

void LCD_StartWriteGRAM(void)
{
	LCD_ClearCS;
	LCD_ClearRS;
	LCD_WriteToDB(0x0022);
	LCD_ClearWR;
	LCD_SetWR;
	LCD_SetRS;
}













































