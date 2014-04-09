#include "i2c3.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_i2c.h"
#include "core_cm4.h"
#include "stm32f4xx.h"


int I2C3_Initialize(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitTypeDef init_port;
	init_port.GPIO_Pin = GPIO_Pin_8;
	init_port.GPIO_Mode = GPIO_Mode_AF;
	init_port.GPIO_Speed = GPIO_Speed_2MHz;
	init_port.GPIO_OType = GPIO_OType_OD;
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
		--len;
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

int I2C1_Initialize(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitTypeDef init_port;
	init_port.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_9;
	init_port.GPIO_Mode = GPIO_Mode_AF;
	init_port.GPIO_Speed = GPIO_Speed_50MHz;
	init_port.GPIO_OType = GPIO_OType_OD;
	init_port.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &init_port);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);

	init_port.GPIO_Pin = GPIO_Pin_4;
	init_port.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOD, &init_port);

	I2C_InitTypeDef i2c_init;
	i2c_init.I2C_ClockSpeed = 50000;
	i2c_init.I2C_Mode = I2C_Mode_I2C;
	i2c_init.I2C_DutyCycle = I2C_DutyCycle_2;
	i2c_init.I2C_OwnAddress1 = 0x01;
	i2c_init.I2C_Ack = I2C_Ack_Disable;
	i2c_init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2C1, &i2c_init);

	I2C_ITConfig(I2C1, I2C_IT_EVT, ENABLE);
	//NVIC_EnableIRQ(I2C1_EV_IRQn);

	I2C_Cmd(I2C1, ENABLE);

	return 0;
}

void I2C1_Write(char slave_addr, char *buf, int len)
{
	I2C_GenerateSTART(I2C1, ENABLE);

	I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT);
	//while(SUCCESS != I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C1, slave_addr, I2C_Direction_Transmitter);

	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	int i = 0;
	while (len > 0) {
		I2C_SendData(I2C1, buf[i]);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
		--len;
	}
	I2C_GenerateSTOP(I2C1, ENABLE);
}

void I2C1_Read(char slave_addr, char *buf, int len)
{
	I2C_GenerateSTART(I2C1, ENABLE);

	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C1, slave_addr, I2C_Direction_Receiver);

	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	int i = 0;
	while (i < len) {
		while( !I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) );
		buf[i++] = I2C_ReceiveData(I2C1);
	}
	I2C_GenerateSTOP(I2C1, ENABLE);
}

/*
void I2C_GenerateSTART(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_GenerateSTOP(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_Send7bitAddress(I2C_TypeDef* I2Cx, uint8_t Address, uint8_t I2C_Direction);

void I2C_SendData(I2C_TypeDef* I2Cx, uint8_t Data);
uint8_t I2C_ReceiveData(I2C_TypeDef* I2Cx);
*/

































