#include "usart.h"

void usart_send(USART_TypeDef *usart, const char *str)
{
    while (*str) {
        while((usart->SR & USART_SR_TXE) == 0);
        usart->DR = *str++;
    }
    while ((usart->SR & USART_SR_TC) == 0);
}