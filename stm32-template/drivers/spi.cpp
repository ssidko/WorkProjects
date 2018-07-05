#include "spi.h"

SpiConfig operator | (SpiConfig lhs, SpiConfig rhs)
{
    return static_cast<SpiConfig> (
        static_cast<std::underlying_type<SpiConfig>::type>(lhs) | 
        static_cast<std::underlying_type<SpiConfig>::type>(rhs)
    );
}

bool spi_enabled(SPI_TypeDef *spi)
{
    return (bool)(SPI1->CR1 &= ~SPI_CR1_SPE);
}

void spi_enable(SPI_TypeDef *spi)
{
    SPI1->CR1 |= SPI_CR1_SPE; 
}

void spi_disable(SPI_TypeDef *spi)
{
    while((spi->SR & SPI_SR_TXE) != 1);
    while((spi->SR & SPI_SR_BSY) != 0);
    SPI1->CR1 &= ~SPI_CR1_SPE; 
}

void spi_send(SPI_TypeDef *spi, uint8_t data)
{
    while ((spi->SR & SPI_SR_TXE) == 0);
    spi->DR = data;
}

uint8_t spi_receive(SPI_TypeDef *spi)
{
    while ((spi->SR & SPI_SR_RXNE) == 0);
    return (uint8_t)spi->DR;
}

uint8_t spi_send_receive(SPI_TypeDef *spi, uint8_t data)
{
    while ((spi->SR & SPI_SR_TXE) == 0);
    spi->DR = data;
    while ((spi->SR & SPI_SR_RXNE) == 0);
    return (uint8_t)spi->DR;
}

void spi_send_buff(SPI_TypeDef *spi, uint8_t *data, size_t size)
{
    spi_send(spi, data[0]);
    for(size_t i = 1; i < size; i++) {
        spi_send_receive(spi, data[i]);
    }
    spi_receive(spi);
}

void spi_wait_for_transfer_complete(SPI_TypeDef *spi)
{
    while((spi->SR & SPI_SR_BSY) != 0);
}

void spi_crc_clear(SPI_TypeDef *spi)
{
    bool enabled = spi_enabled(spi);
    if (enabled) {
        spi_disable(spi);
    }

    spi->CR1 &= ~SPI_CR1_CRCEN;
    spi->CR1 |= SPI_CR1_CRCEN;
    
    if (enabled) {
        spi_enable(spi);
    }
}