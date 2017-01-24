#include "spi.h"

void SPI_SendByte(SPI_TypeDef *spi, uint8_t byte)
{
	while (!IS_SPI_TRX_BUFFER_EMPTY(spi)) {}

	spi->DR = byte;

	while (!IS_SPI_TRX_BUFFER_EMPTY(spi)) {}
	while (IS_SPI_BUSY(spi)) {}
}

void SPI_SendData(SPI_TypeDef *spi, uint8_t *data, uint32_t size)
{
	if (!data || !size) {
		return;
	}

	while (size) {
		while (!IS_SPI_TRX_BUFFER_EMPTY(spi)) {}
		spi->DR = *data;
		data++;
		size--;
	}

	while (!IS_SPI_TRX_BUFFER_EMPTY(spi)) {}
	while (IS_SPI_BUSY(spi)) {}
}
