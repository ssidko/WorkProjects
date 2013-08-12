#include "SPI3.h"


void SPI3_Configure(void)
{
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitTypeDef init_port;
	init_port.GPIO_Pin = SPI3_MISO_Pin;
	init_port.GPIO_Mode = GPIO_Mode_AF;
	init_port.GPIO_Speed = GPIO_Speed_50MHz;
	init_port.GPIO_OType = GPIO_OType_PP;
	init_port.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(SPI3_MISO_Port, &init_port);

	init_port.GPIO_Pin = SPI3_MOSI_Pin;
	GPIO_Init(SPI3_MOSI_Port, &init_port);

	init_port.GPIO_Pin = SPI3_SCLK_Pin;
	GPIO_Init(SPI3_SCLK_Port, &init_port);

	init_port.GPIO_Pin = SPI3_SS_Pin;
	GPIO_Init(SPI3_SS_Port, &init_port);

	GPIO_PinAFConfig(SPI3_MISO_Port, SPI3_MISO_Pin, GPIO_AF_SPI3);
	GPIO_PinAFConfig(SPI3_MOSI_Port, SPI3_MOSI_Pin, GPIO_AF_SPI3);
	GPIO_PinAFConfig(SPI3_SCLK_Port, SPI3_SCLK_Pin, GPIO_AF_SPI3);
	GPIO_PinAFConfig(SPI3_SS_Port, SPI3_SS_Pin, GPIO_AF_SPI3);

}

void SPI3_Initialize(void)
{
	SPI_InitTypeDef spi_init;
	SPI_StructInit(&spi_init);
	spi_init.SPI_Mode = SPI_Mode_Master;
	spi_init.SPI_DataSize = SPI_DataSize_8b;
	spi_init.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(SPI3, &spi_init);
	SPI_Cmd(SPI3, ENABLE);
}

void SPI3_Testing(void)
{

}
