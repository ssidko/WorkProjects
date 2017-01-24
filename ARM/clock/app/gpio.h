#ifndef _GPIO_H
#define _GPIO_H

#include "stm32f1xx.h"

#define GPIOA_ClockEnable()						(RCC->APB2ENR |= RCC_APB2ENR_IOPAEN)
#define GPIOB_ClockEnable()						(RCC->APB2ENR |= RCC_APB2ENR_IOPBEN)
#define GPIOC_ClockEnable()						(RCC->APB2ENR |= RCC_APB2ENR_IOPCEN)
#define GPIOD_ClockEnable()						(RCC->APB2ENR |= RCC_APB2ENR_IOPDEN)
#define GPIOE_ClockEnable()						(RCC->APB2ENR |= RCC_APB2ENR_IOPEEN)
#define GPIOF_ClockEnable()						(RCC->APB2ENR |= RCC_APB2ENR_IOPFEN)
#define GPIOG_ClockEnable()						(RCC->APB2ENR |= RCC_APB2ENR_IOPGEN)

#define GPIO_PinReset(port, pin)				(port->BSRR = (1 << pin) << 16)
#define GPIO_PinSet(port, pin)					(port->BSRR = (1 << pin))
#define GPIO_ToglePin(port, pin)				(port->ODR ^= (1 << pin))

enum {
	Pin0, Pin1, Pin2, Pin3, Pin4, Pin5, Pin6, Pin7,
	Pin8, Pin9, Pin10, Pin11, Pin12, Pin13, Pin14, Pin15
};

/*
	MODEy[1:0]: Port x mode bits (y= 0 .. 7)
	These bits are written by software to configure the corresponding I/O port.
	Refer to Table 20: Port bit configuration table.
	00: Input mode (reset state)
	01: Output mode, max speed 10 MHz.
	10: Output mode, max speed 2 MHz.
	11: Output mode, max speed 50 MHz.
*/

/* GPIO_CR_MODE */
enum {
	Input				= 0,
	Out_10Mhz			= 1,
	Out_2Mhz			= 2,
	Out_50Mhz			= 3
};

/*
	In input mode (MODE[1:0]=00):
	00: Analog mode
	01: Floating input (reset state)
	10: Input with pull-up / pull-down
	11: Reserved

	In output mode (MODE[1:0] > 00):
	00: General purpose output push-pull
	01: General purpose output Open-drain
	10: Alternate function output Push-pull
	11: Alternate function output Open-drain
*/

/* GPIO_CR_CNF In input mode (MODE[1:0]=00 */
enum {
	In_Analog			= 0 << 2,
	In_Float			= 1 << 2,
	In_PullUpPullDown	= 2 << 2
};

/* GPIO_CR_CNF In output mode (MODE[1:0] > 00 */
enum {
	Out_PushPull		= 0 << 2,
	Out_OpenDrain		= 1 << 2,
	Out_AF_PushPull		= 2 << 2,
	Out_AF_OpenDrain	= 3 << 2
};

#endif // _GPIO_H
