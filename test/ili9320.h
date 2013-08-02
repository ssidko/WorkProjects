#ifndef _ILI9320_H
#define _ILI9320_H

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"

void LCD_LineConfig(void);
inline void LCD_LineCS(FunctionalState new_state);
inline void LCD_LineRS(FunctionalState new_state);
inline void LCD_LineRD(FunctionalState new_state);
inline void LCD_LineWR(FunctionalState new_state);
inline void LCD_LineRESET(FunctionalState new_state);
void LCD_WriteToDB(unsigned short val); // Write to Data BUS (Db0 - Db15)


#endif // _ILI9320_H
