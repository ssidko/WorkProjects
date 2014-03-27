#include "ili9320.h"

void LCD_LineConfig(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	// Data Bus PE0-PE15
	GPIO_InitTypeDef init_port;
	init_port.GPIO_Pin = GPIO_Pin_All;
	init_port.GPIO_Mode = GPIO_Mode_OUT;
	init_port.GPIO_Speed = GPIO_Speed_100MHz;
	init_port.GPIO_OType = GPIO_OType_PP;
	init_port.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(LCD_DBUS_PORT, &init_port);

	// LCD_CS, LCD_RS, LCD_RD, LCD_WR, LCD_RESET.
	init_port.GPIO_Pin = LCD_CS_PIN | LCD_RS_PIN | LCD_RD_PIN | LCD_WR_PIN | LCD_RESET_PIN;
	GPIO_Init(GPIOB, &init_port);
}

void LCD_ConfigureDBusOUT(void)
{
	GPIO_InitTypeDef init_port;
	init_port.GPIO_Pin = GPIO_Pin_All;
	init_port.GPIO_Mode = GPIO_Mode_OUT;
	init_port.GPIO_Speed = GPIO_Speed_100MHz;
	init_port.GPIO_OType = GPIO_OType_PP;
	init_port.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(LCD_DBUS_PORT, &init_port);
}

void LCD_ConfigureDBusIN(void)
{
	GPIO_InitTypeDef init_port;
	init_port.GPIO_Pin = GPIO_Pin_All;
	init_port.GPIO_Mode = GPIO_Mode_IN;
	init_port.GPIO_Speed = GPIO_Speed_100MHz;
	init_port.GPIO_OType = GPIO_OType_PP;
	init_port.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(LCD_DBUS_PORT, &init_port);
}

void LCD_WriteToDB(unsigned short val)
{
	LCD_DBUS_PORT->ODR = val;
}

unsigned short LCD_ReadFromDB(void)
{
	return ((unsigned short)LCD_DBUS_PORT->IDR);
}

void LCD_ili9320_Reset(void)
{
	LCD_SetRESET;
	Delay(10);
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

    LCD_ili9320_WriteToReg(0x00,0x0000);
	LCD_ili9320_WriteToReg(0x01,0x0100);	/* Driver Output Contral */
	LCD_ili9320_WriteToReg(0x02,0x0700);	/* LCD Driver Waveform Contral */
	LCD_ili9320_WriteToReg(0x03,0x1038);	/* Entry Mode Set */
	LCD_ili9320_WriteToReg(0x04,0x0000);	/* Scalling Contral */
    LCD_ili9320_WriteToReg(0x08,0x0202);	/* Display Contral */
	LCD_ili9320_WriteToReg(0x09,0x0000);	/* Display Contral 3.(0x0000) */
	LCD_ili9320_WriteToReg(0x0a,0x0000);	/* Frame Cycle Contal.(0x0000) */
    LCD_ili9320_WriteToReg(0x0c,(1<<0));	/* Extern Display Interface Contral */
	LCD_ili9320_WriteToReg(0x0d,0x0000);	/* Frame Maker Position */
	LCD_ili9320_WriteToReg(0x0f,0x0000);	/* Extern Display Interface Contral 2. */

    Delay(100);  /* delay 100 ms */
	LCD_ili9320_WriteToReg(0x07,0x0101);	/* Display Contral */
    Delay(100);  /* delay 100 ms */

	LCD_ili9320_WriteToReg(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));	/* Power Control 1.(0x16b0)	*/
	LCD_ili9320_WriteToReg(0x11,0x0007);								/* Power Control 2 */
	LCD_ili9320_WriteToReg(0x12,(1<<8)|(1<<4)|(0<<0));					/* Power Control 3.(0x0138)	*/
	LCD_ili9320_WriteToReg(0x13,0x0b00);								/* Power Control 4 */
	LCD_ili9320_WriteToReg(0x29,0x0000);								/* Power Control 7 */

	LCD_ili9320_WriteToReg(0x2b,(1<<14)|(1<<4));

	LCD_ili9320_WriteToReg(0x50,0);      	/* Set X Start */
	LCD_ili9320_WriteToReg(0x51,239);	    /* Set X End */
	LCD_ili9320_WriteToReg(0x52,0);	    	/* Set Y Start */
	LCD_ili9320_WriteToReg(0x53,319);		/* Set Y End */

	LCD_ili9320_WriteToReg(0x60,0xa700);	/* Driver Output Control */
	LCD_ili9320_WriteToReg(0x61,0x0001);	/* Driver Output Control */
	LCD_ili9320_WriteToReg(0x6a,0x0000);	/* Vertical Srcoll Control */

	LCD_ili9320_WriteToReg(0x80,0x0000);	/* Display Position? Partial Display 1 */
	LCD_ili9320_WriteToReg(0x81,0x0000);	/* RAM Address Start? Partial Display 1 */
	LCD_ili9320_WriteToReg(0x82,0x0000);	/* RAM Address End-Partial Display 1 */
	LCD_ili9320_WriteToReg(0x83,0x0000);	/* Displsy Position? Partial Display 2 */
	LCD_ili9320_WriteToReg(0x84,0x0000);	/* RAM Address Start? Partial Display 2 */
	LCD_ili9320_WriteToReg(0x85,0x0000);	/* RAM Address End? Partial Display 2 */

    LCD_ili9320_WriteToReg(0x90,(0<<7)|(16<<0));	/* Frame Cycle Contral.(0x0013)	*/
	LCD_ili9320_WriteToReg(0x92,0x0000);	/* Panel Interface Contral 2.(0x0000) */
	LCD_ili9320_WriteToReg(0x93,0x0001);	/* Panel Interface Contral 3. */
    LCD_ili9320_WriteToReg(0x95,0x0110);	/* Frame Cycle Contral.(0x0110)	*/
	LCD_ili9320_WriteToReg(0x97,(0<<8));
	LCD_ili9320_WriteToReg(0x98,0x0000);	/* Frame Cycle Contral */

    LCD_ili9320_WriteToReg(0x07,0x0173);

    int i = 0;
	unsigned short colour;


	while (1) {
		colour = White;
		SetCursor(0, 0);
		LCD_StartWriteGRAM();
		i = 0;
		for (; i < 320*240; ++i) {
			LCD_WriteToDB(colour);
			LCD_ClearWR;
			LCD_ClearWR;
			LCD_SetWR;
		}
		LCD_SetCS;
		Delay(500);

		colour = Blue;
		SetCursor(0, 0);
		LCD_StartWriteGRAM();
		i = 0;
		for (; i < 320*240; ++i) {
			LCD_WriteToDB(colour);
			LCD_ClearWR;
			LCD_ClearWR;
			LCD_SetWR;
		}
		LCD_SetCS;
		Delay(500);

		colour = Red;
		SetCursor(0, 0);
		LCD_StartWriteGRAM();
		i = 0;
		for (; i < 320*240; ++i) {
			LCD_WriteToDB(colour);
			LCD_ClearWR;
			LCD_ClearWR;
			LCD_SetWR;
		}
		LCD_SetCS;
		Delay(500);

		colour = Green;
		SetCursor(0, 0);
		LCD_StartWriteGRAM();
		i = 0;
		for (; i < 320*240; ++i) {
			LCD_WriteToDB(colour);
			LCD_ClearWR;
			LCD_ClearWR;
			LCD_SetWR;
		}
		LCD_SetCS;
		Delay(500);
	}
}

void LCD_ili9320_WriteToReg(unsigned short reg_index, unsigned short value)
{
	LCD_ClearCS;
	LCD_ClearRS;
	LCD_WriteToDB(reg_index);
	LCD_ClearWR;
	LCD_SetWR;
	LCD_SetRS;
	LCD_WriteToDB(value);
	LCD_ClearWR;
	LCD_SetWR;
	LCD_SetCS;
}

unsigned short LCD_ili9320_ReadFromReg(unsigned short reg_index)
{
	unsigned short data = 0;

	LCD_ClearCS;
	LCD_ClearRS;
	LCD_WriteToDB(reg_index);
	LCD_ClearWR;
	LCD_SetWR;
	LCD_SetRS;
	LCD_ConfigureDBusIN();
	LCD_ClearRD;
	LCD_SetRD;
	data = LCD_ReadFromDB();
	LCD_SetCS;
	LCD_ConfigureDBusOUT();

	return data;
}

unsigned short LCD_ili9320_8bit_ReadFromReg(unsigned short reg_index)
{
	unsigned short data = 0;

	LCD_ClearCS;
	LCD_ClearRS;
	LCD_WriteToDB(0x00);
	LCD_ClearWR;
	LCD_SetWR;
	LCD_WriteToDB(reg_index);
	LCD_ClearWR;
	LCD_SetWR;

	LCD_ConfigureDBusIN();
	LCD_ClearRD;
	LCD_ClearRD;
	data = LCD_ReadFromDB() << 8;
	LCD_ClearRD;
	LCD_ClearRD;
	data |= LCD_ReadFromDB();
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

void SetCursor(unsigned short x, unsigned short y)
{
	LCD_ili9320_WriteToReg(0x20, y);
	LCD_ili9320_WriteToReg(0x21, x);
}













































