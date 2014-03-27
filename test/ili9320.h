#ifndef _ILI9320_H
#define _ILI9320_H

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"

#include "board.h"

#define LCD_DBUS_PORT					GPIOE

#define LCD_CS_PIN						GPIO_Pin_11
#define LCD_CS_PORT						GPIOB

#define LCD_RS_PIN						GPIO_Pin_12
#define LCD_RS_PORT						GPIOB

#define LCD_RD_PIN						GPIO_Pin_14
#define LCD_RD_PORT						GPIOB

#define LCD_WR_PIN						GPIO_Pin_13
#define LCD_WR_PORT						GPIOB

#define LCD_RESET_PIN					GPIO_Pin_10
#define LCD_RESET_PORT					GPIOB

#define LCD_ClearCS						GPIO_ResetBits(LCD_CS_PORT, LCD_CS_PIN);
#define LCD_SetCS						GPIO_SetBits(LCD_CS_PORT, LCD_CS_PIN);

#define LCD_ClearRS						GPIO_ResetBits(LCD_RS_PORT, LCD_RS_PIN);
#define LCD_SetRS						GPIO_SetBits(LCD_RS_PORT, LCD_RS_PIN);

#define LCD_ClearRD						GPIO_ResetBits(LCD_RD_PORT, LCD_RD_PIN);
#define LCD_SetRD						GPIO_SetBits(LCD_RD_PORT, LCD_RD_PIN);

#define LCD_ClearWR						GPIO_ResetBits(LCD_WR_PORT, LCD_WR_PIN);
#define LCD_SetWR						GPIO_SetBits(LCD_WR_PORT, LCD_WR_PIN);

#define LCD_ClearRESET					GPIO_ResetBits(LCD_RESET_PORT, LCD_RESET_PIN);
#define LCD_SetRESET					GPIO_SetBits(LCD_RESET_PORT, LCD_RESET_PIN);

/* LCD color */
#define White							0xFFFF
#define Black							0x0000
#define Grey							0xF7DE
#define Blue							0x001F
#define Blue2							0x051F
#define Red								0xF800
#define Magenta							0xF81F
#define Green							0x07E0
#define Cyan							0x7FFF
#define Yellow							0xFFE0

void LCD_LineConfig(void);
void LCD_ConfigureDBusOUT(void);
void LCD_ConfigureDBusIN(void);
void LCD_WriteToDB(unsigned short val); // Write to Data BUS (Db0 - Db15)
unsigned short LCD_ReadFromDB(void); // Read from Data BUS
void LCD_ili9320_Reset(void);
void LCD_ili9320_Initialize(void);
void LCD_ili9320_WriteToReg(unsigned short reg_index, unsigned short value);
unsigned short LCD_ili9320_8bit_ReadFromReg(unsigned short reg_index);
unsigned short LCD_ili9320_ReadFromReg(unsigned short reg_index);
void LCD_StartWriteGRAM(void);

void SetCursor(unsigned short x, unsigned short y);


#endif // _ILI9320_H
