#ifndef _SPI_H
#define _SPI_H

#include "stm32f1xx.h"

#define SPI1_ClockEnable()						(RCC->APB2ENR |= RCC_APB2ENR_SPI1EN)
#define SPI_Enable(spi)							(spi->CR1 |= SPI_CR1_SPE)

#define IS_SPI_BUSY(spi)						((spi->SR & SPI_SR_BSY) != 0)
#define IS_SPI_TRX_BUFFER_EMPTY(spi)			((spi->SR & SPI_SR_TXE) != 0)
#define IS_SPI_RX_BUFFER_NOT_EMPTY(spi)			((spi->SR & SPI_SR_RXNE) != 0)

void SPI_SendByte(SPI_TypeDef *spi, uint8_t byte);
void SPI_SendData(SPI_TypeDef *spi, uint8_t *data, uint32_t size);

#endif // _SPI_H
