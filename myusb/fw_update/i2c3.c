#include "i2c3.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_i2c.h"

int I2C3_Initialize(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitTypeDef init_port;
	init_port.GPIO_Pin = GPIO_Pin_8;
	init_port.GPIO_Mode = GPIO_Mode_AF;
	init_port.GPIO_Speed = GPIO_Speed_2MHz;
	init_port.GPIO_OType = GPIO_OType_PP;
	init_port.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_Init(GPIOA, &init_port);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_I2C3);

	init_port.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOC, &init_port);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_I2C3);

	I2C_InitTypeDef i2c_init;
	i2c_init.I2C_ClockSpeed = 200000;
	i2c_init.I2C_Mode = I2C_Mode_I2C;
	i2c_init.I2C_DutyCycle = I2C_DutyCycle_2;
	i2c_init.I2C_OwnAddress1 = 0x00;
	i2c_init.I2C_Ack = I2C_Ack_Disable;
	i2c_init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2C3, &i2c_init);

	I2C_Cmd(I2C3, ENABLE);

	return 0;
}

void I2C3_Write(char slave_addr, char *buf, int len)
{
	I2C_GenerateSTART(I2C3, ENABLE);
	I2C_Send7bitAddress(I2C3, slave_addr, I2C_Direction_Transmitter);
	int i = 0;
	while (len > 0) {
		I2C_SendData(I2C3, buf[i]);
	}
	I2C_GenerateSTOP(I2C3, ENABLE);
}

void I2C3_Read(char slave_addr, char *buf, int len)
{
	I2C_GenerateSTART(I2C3, ENABLE);
	I2C_Send7bitAddress(I2C3, slave_addr, I2C_Direction_Receiver);
	while (len > 0) {

	}
	I2C_GenerateSTOP(I2C3, ENABLE);
}

/*
void I2C_GenerateSTART(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_GenerateSTOP(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_Send7bitAddress(I2C_TypeDef* I2Cx, uint8_t Address, uint8_t I2C_Direction);

void I2C_SendData(I2C_TypeDef* I2Cx, uint8_t Data);
uint8_t I2C_ReceiveData(I2C_TypeDef* I2Cx);
*/

































