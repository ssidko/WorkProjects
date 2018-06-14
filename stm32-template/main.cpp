#include <stdint.h>
#include "stm32f1xx.h"


uint32_t systick_counter = 0;
extern "C" int SysTick_Handler()
{
    ++systick_counter;
}

void delay(uint32_t ms)
{
    uint32_t start = systick_counter;
    while ((systick_counter - start) < ms);
}

/*
void rcc_enable_hse();
void rcc_wait_for_ready_hse();
void rcc_enable_pll();
void rcc_wait_for_ready_pll();
void rcc_pllsrc_hse();
*/

uint32_t apb1clk = 0;
uint32_t apb2clk = 0;

void system_clock_setup(void)
{
    // Enable HSE and wait for ready
    RCC->CR |= RCC_CR_HSEON;
    while ((RCC->CR & RCC_CR_HSERDY) == 0);

    // Eenable clock security system 
    RCC->CR |= RCC_CR_CSSON;

    RCC->CFGR &= ~RCC_CFGR_PLLXTPRE;

    RCC->CFGR |= RCC_CFGR_PLLSRC;

    RCC->CFGR &= (~RCC_CFGR_PLLMULL);
    RCC->CFGR |= RCC_CFGR_PLLMULL9;

    // Enable PLL and wait for ready
    RCC->CR |= RCC_CR_PLLON;
    while ((RCC->CR & RCC_CR_PLLRDY) == 0);

    // AHB prescaller = 1
    RCC->CFGR &= (~RCC_CFGR_HPRE);
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

    // APB1 prescaller = 2
    RCC->CFGR &= (~RCC_CFGR_PPRE1);
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

    // APB2 prescaller = 1
    RCC->CFGR &= (~RCC_CFGR_PPRE2);
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

    // config flash
    FLASH->ACR = FLASH_ACR_LATENCY_2;

    // Select pll as system clock
    RCC->CFGR &= (~RCC_CFGR_SW);
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    // Wait 
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

    SystemCoreClockUpdate();
    apb1clk = SystemCoreClock/2;
    apb2clk = SystemCoreClock;
}

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

inline void rcc_gpioa_enable()
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; 
}

inline void rcc_gpioc_enable()
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; 
}

inline void rcc_usart1_enable()
{
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;   
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

void led_setup()
{
    // PC-13
    //RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    //GPIOC->CRH &= ~(GPIO_CRH_MODE13|GPIO_CRH_CNF13);
    //GPIOC->CRH |= (PinConfig::Output_2MHz_PushPull<<5*4);

    rcc_gpioc_enable();
    gpio_pin_configure(GPIOC, Pin::Pin13, PinConfig::Output_2MHz_PushPull);
}

void led_on()
{
    GPIOC->BSRR = GPIO_BSRR_BR13;
}

void led_off()
{
    GPIOC->BSRR = GPIO_BSRR_BS13;
}

void usart1_setup()
{
    rcc_gpioa_enable();
    // USART1_Tx
    gpio_pin_configure(GPIOA, Pin::Pin9, PinConfig::OutputAF_10MHz_PushPull);
    // USART1_Rx
    gpio_pin_configure(GPIOA, Pin::Pin10, PinConfig::Input_Floating);

    rcc_usart1_enable();

    uint32_t boud_rate = 115200;
    uint32_t mantissa =  apb2clk / boud_rate;
    uint32_t fraction = mantissa % 16;

    USART1->CR1 |= USART_CR1_UE;
    USART1->CR1 |= USART_CR1_TE;

    USART1->BRR = 0;
    USART1->BRR = ((mantissa / 16) << 4)|fraction;

    // 1 Start bit, 8 Data bits
    USART1->CR1 &= ~USART_CR1_M;

    // 1 Stop bit
    USART1->CR2 &= ~USART_CR2_STOP;
    USART1->CR2 |=  0b00 << USART_CR2_STOP_Pos;
}

void usart_sent(USART_TypeDef *usart, const char *str)
{
    while (*str) {
        while((usart->SR & USART_SR_TXE) == 0);
        usart->DR = *str++;
    }
    while ((usart->SR & USART_SR_TC) == 0);
}

extern "C" int main()
{
    system_clock_setup();
    led_setup();
    usart1_setup();

    SysTick_Config(SystemCoreClock/1000);

    int x = 0;
    while (true) {
        led_on();
        usart_sent(USART1, "Len on\n");
        delay(1000);
        led_off();
        usart_sent(USART1, "Len off\n");
        delay(1000);
        x++;
    }

    return 0;
}