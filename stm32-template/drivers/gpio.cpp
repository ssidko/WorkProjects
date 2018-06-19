#include "gpio.h"

PinFlag operator|(PinFlag lhs, PinFlag rhs)
{
    return static_cast<PinFlag> (
        static_cast<std::underlying_type<PinFlag>::type>(lhs) | 
        static_cast<std::underlying_type<PinFlag>::type>(rhs)
    );
}

void gpio_pin_configure(GPIO_TypeDef *port, Pin pin, PinConfig conf)
{
    if (pin < 8) {
        port->CRL &= ~(0x0f << pin*4);
        port->CRL |= (conf << pin*4);
    } else {
        port->CRH &= ~(0x0f << (pin - 8)*4);
        port->CRH |= (conf << (pin - 8)*4);
    }
}

void gpio_pin_pullup(GPIO_TypeDef *port, PinFlag pin)
{
    GPIOA->BSRR = static_cast<uint32_t>(pin); // pull-up
}

void gpio_pin_pulldown(GPIO_TypeDef *port, PinFlag pin)
{
    GPIOA->BSRR = (static_cast<uint32_t>(pin)) << 16; // pull-up
}