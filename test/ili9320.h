#ifndef _ILI9320_H
#define _ILI9320_H

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"

#include "board.h"

/* LCD color */
#define White          0xFFFF
#define Black          0x0000
#define Grey           0xF7DE
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

void LCD_LineConfig(void);
inline void LCD_LineCS(FunctionalState new_state);
inline void LCD_LineRS(FunctionalState new_state);
inline void LCD_LineRD(FunctionalState new_state);
inline void LCD_LineWR(FunctionalState new_state);
inline void LCD_LineRESET(FunctionalState new_state);
void LCD_WriteToDB(unsigned short val); // Write to Data BUS (Db0 - Db15)
unsigned short LCD_ReadFromDB(void); // Read from Date BUS
void LCD_ili9320_Reset(void);
void LCD_ili9320_Initialize(void);
void LCD_ili9320_WriteToReg(unsigned short reg_index, unsigned short value);
unsigned short LCD_ili9320_ReadFromReg(unsigned short reg_index);

void LCD_StartWriteGRAM(void);


#endif // _ILI9320_H
