#ifndef _SPI_H
#define _SPI_H

#include "stm32f1xx.h"

#define SPI1_ClockEnable()						(RCC->APB2ENR |= RCC_APB2ENR_SPI1EN)
#define SPI_Enable(spi)							(spi->CR1 |= SPI_CR1_SPE)

#define IS_SPI_BUSY(spi)						((spi->SR & SPI_SR_BSY) != 0)
#define IS_SPI_TRX_BUFFER_EMPTY(spi)			((spi->SR & SPI_SR_TXE) != 0)
#define IS_SPI_RX_BUFFER_NOT_EMPTY(spi)			((spi->SR & SPI_SR_RXNE) != 0)

/* SPI_CR1 */
enum {
	/* CPHA */
	CPHA_0				= 0 << SPI_CR1_CPHA_Pos,
	CPHA_1				= 1 << SPI_CR1_CPHA_Pos,
	/* CPOL */
	CPOL_0				= 0 << SPI_CR1_CPOL_Pos,
	CPOL_1				= 1 << SPI_CR1_CPOL_Pos,
	/* MSTR: */
	SPI_Slave			= 0 << SPI_CR1_MSTR_Pos,
	SPI_Master			= 1 << SPI_CR1_MSTR_Pos,
	/* BR[2:0]: Baud rate control */
	Fpclk_2				= 0 << SPI_CR1_BR_Pos, // fPCLK/2
	Fpclk_4				= 1 << SPI_CR1_BR_Pos,
	Fpclk_8				= 2 << SPI_CR1_BR_Pos,
	Fpclk_16			= 3 << SPI_CR1_BR_Pos,
	Fpclk_32			= 4 << SPI_CR1_BR_Pos,
	Fpclk_64			= 5 << SPI_CR1_BR_Pos,
	Fpclk_128			= 6 << SPI_CR1_BR_Pos,
	Fpclk_256			= 7 << SPI_CR1_BR_Pos, // fPCLK/256
	/* SPE: SPI enable */
	SPI_Disable			= 0 << SPI_CR1_SPE_Pos,
	SPI_Enable			= 1 << SPI_CR1_SPE_Pos,
	/* LSBFIRST: Frame format */
	MSB_First			= 0 << SPI_CR1_LSBFIRST_Pos,
	LSB_First			= 1 << SPI_CR1_LSBFIRST_Pos,
	/* SSI: Internal slave select */
	SSI_Clear			= 0 << SPI_CR1_SSI_Pos,
	SSI_Set				= 1 << SPI_CR1_SSI_Pos,
	/* SSM: Software slave management */
	SSM_Disable			= 0 << SPI_CR1_SSM_Pos,
	SSM_Enable			= 1 << SPI_CR1_SSM_Pos,
	/* RXONLY: Receive only */
	FullDuplex			= 0 << SPI_CR1_RXONLY_Pos, // 0: Full duplex (Transmit and receive)
	ReceiveOnly			= 1 << SPI_CR1_RXONLY_Pos, // 1: Output disabled (Receive-only mode)
	/* DFF: Data frame format */
	Frame_8bit			= 0 << SPI_CR1_DFF_Pos,
	Frame_16bit			= 1 << SPI_CR1_DFF_Pos,
	/* CRCNEXT: CRC transfer next */
	DataPhase			= 0 << SPI_CR1_CRCNEXT_Pos,
	CRC_Phase			= 1 << SPI_CR1_CRCNEXT_Pos,
	/* CRCEN: Hardware CRC calculation enable */
	CRC_Disable			= 0 << SPI_CR1_CRCEN_Pos,
	CRC_Enable			= 1 << SPI_CR1_CRCEN_Pos,
	/* BIDIOE: Output enable in bidirectional mode */
	Output_Disable		= 0 << SPI_CR1_BIDIOE_Pos,
	Output_Enable		= 1 << SPI_CR1_BIDIOE_Pos,
	/* BIDIMODE: Bidirectional data mode enable */
	Unidirect_TwoWire	= 0 << SPI_CR1_BIDIMODE_Pos,
	Bidirect_OneWire	= 1 << SPI_CR1_BIDIMODE_Pos
};

void SPI_SendByte(SPI_TypeDef *spi, uint8_t byte);
void SPI_SendData(SPI_TypeDef *spi, uint8_t *data, uint32_t size);

#endif // _SPI_H
