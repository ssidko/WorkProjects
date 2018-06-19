#ifndef __SPI_H
#define __SPI_H

#include <stdlib.h>
#include <type_traits>

#include "stm32f1xx.h"

enum class SpiConfig {
    DFF_8bit = 0 << SPI_CR1_DFF_Pos,
    DFF_16bit = 1 << SPI_CR1_DFF_Pos,

    MSB_first = 0 << SPI_CR1_LSBFIRST_Pos,
    LSB_first = 1 << SPI_CR1_LSBFIRST_Pos,
    
    BaudRate_div2 = 0b000 << SPI_CR1_BR_Pos,
    BaudRate_div4 = 0b001 << SPI_CR1_BR_Pos,
    BaudRate_div8 = 0b010 << SPI_CR1_BR_Pos,
    BaudRate_div16 = 0b011 << SPI_CR1_BR_Pos,
    BaudRate_div32 = 0b100 << SPI_CR1_BR_Pos,
    BaudRate_div64 = 0b101 << SPI_CR1_BR_Pos,
    BaudRate_div128 = 0b110 << SPI_CR1_BR_Pos,
    BaudRate_div256 = 0b111 << SPI_CR1_BR_Pos,

    Slave = 0 << SPI_CR1_MSTR_Pos,
    Master = 1 << SPI_CR1_MSTR_Pos,

    CPOL_0 = 0 << SPI_CR1_CPOL_Pos,
    CPOL_1 = 1 << SPI_CR1_CPOL_Pos,

    CPHA_0 = 0 << SPI_CR1_CPHA_Pos,
    CPHA_1 = 1 << SPI_CR1_CPHA_Pos,
};

SpiConfig operator | (SpiConfig lhs, SpiConfig rhs);

//void spi_configur(SPI_TypeDef *spi);

bool spi_enabled(SPI_TypeDef *spi);
void spi_enable(SPI_TypeDef *spi);
void spi_disable(SPI_TypeDef *spi);

void spi_send(SPI_TypeDef *spi, uint8_t data);
uint8_t spi_receive(SPI_TypeDef *spi);
uint8_t spi_send_receive(SPI_TypeDef *spi, uint8_t data);
void spi_send_buff(SPI_TypeDef *spi, uint8_t *data, size_t size);

void spi_wait_for_transfer_complete(SPI_TypeDef *spi);

void spi_crc_clear(SPI_TypeDef *spi);

#endif // __SPI_H