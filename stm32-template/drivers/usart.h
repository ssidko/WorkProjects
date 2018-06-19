#ifndef __USART_H
#define __USART_H

#include "stm32f1xx.h"

void usart_send(USART_TypeDef *usart, const char *str);

#endif // 