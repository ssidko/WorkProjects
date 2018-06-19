#ifndef __GPIO_H
#define __GPIO_H

#include <type_traits>
#include "stm32f1xx.h"

enum PinConfig {
    Input_Analog    = 0b0000,
    Input_Floating  = 0b0100,
    Input_PuPd      = 0b1000,

    // General purpose output
    Output_2MHz_PushPull    = 0b0010,
    Output_2MHz_OpenDrain   = 0b0110,
    Output_10MHz_PushPull   = 0b0001,
    Output_10MHz_OpenDrain  = 0b0101,
    Output_50MHz_PushPull   = 0b0011,
    Output_50MHz_OpenDrain  = 0b0111,

    // Alternate Function output
    OutputAF_2MHz_PushPull   = 0b1010,
    OutputAF_2MHz_OpenDrain  = 0b1110,
    OutputAF_10MHz_PushPull  = 0b1001,
    OutputAF_10MHz_OpenDrain = 0b1101,
    OutputAF_50MHz_PushPull  = 0b1011,
    OutputAF_50MHz_OpenDrain = 0b1111,
};

enum Pin {
    Pin0 = 0,
    Pin1, Pin2, Pin3, Pin4, Pin5, Pin6, Pin7, Pin8,
    Pin9, Pin10, Pin11, Pin12, Pin13, Pin14, Pin15
};

enum class PinFlag {
    Pin_0 = 1 << 0,
    Pin_1 = 1 << 1,
    Pin_2 = 1 << 2,
    Pin_3 = 1 << 3,
    Pin_4 = 1 << 4,
    Pin_5 = 1 << 5,
    Pin_6 = 1 << 6,
    Pin_7 = 1 << 7,
    Pin_8 = 1 << 8,
    Pin_9 = 1 << 9,
    Pin_10 = 1 << 10,
    Pin_11 = 1 << 11,
    Pin_12 = 1 << 12,
    Pin_13 = 1 << 13,
    Pin_14 = 1 << 14,
    Pin_15 = 1 << 15,
};

PinFlag operator|(PinFlag lhs, PinFlag rhs);


void gpio_pin_configure(GPIO_TypeDef *port, Pin pin, PinConfig conf);
void gpio_pin_pullup(GPIO_TypeDef *port, PinFlag pin);
void gpio_pin_pulldown(GPIO_TypeDef *port, PinFlag pin);

#endif // __GPIO_H