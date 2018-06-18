#include <stdlib.h>
#include <stdint.h>
#include <type_traits>
#include <functional>
#include "stm32f1xx.h"


uint32_t systick_counter = 0;
extern "C" int SysTick_Handler()
{
    ++systick_counter;
}

void delay(uint32_t ms)
{
    uint32_t start = systick_counter;
    uint32_t status = SysTick->CTRL;
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

enum class PinFlag {
    Pin_0 = 1,
    Pin_1 = 1 << 1,
    Pin_2 = 1 << 2,
    Pin_3 = 1 << 3,
    Pin_4 = 1 << 4,
};

PinFlag operator|(PinFlag lhs, PinFlag rhs)
{
    return static_cast<PinFlag> (
        static_cast<std::underlying_type<PinFlag>::type>(lhs) | 
        static_cast<std::underlying_type<PinFlag>::type>(rhs)
    );
}

void gpio_pins_configure(GPIO_TypeDef *port, PinFlag pin_mask, PinConfig conf)
{
    uint32_t mask = static_cast<uint32_t>(pin_mask);
    mask++;
    mask++;
}

void rcc_gpioa_enable()
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; 
}

void rcc_gpioc_enable()
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; 
}

void rcc_usart1_enable()
{
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;   
}

void rcc_spi1_enable()
{
       RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;  
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

void usart_send(USART_TypeDef *usart, const char *str)
{
    while (*str) {
        while((usart->SR & USART_SR_TXE) == 0);
        usart->DR = *str++;
    }
    while ((usart->SR & USART_SR_TC) == 0);
}

void spi_enable(SPI_TypeDef *spi)
{
    SPI1->CR1 |= SPI_CR1_SPE; 
}

void spi_disable(SPI_TypeDef *spi)
{
    while((spi->SR & SPI_SR_BSY) != 0);
    SPI1->CR1 &= ~SPI_CR1_SPE; 
}

void spi1_setup()
{
    rcc_gpioa_enable();
    rcc_spi1_enable();

    // PA7 - SPI1_MOSI
    gpio_pin_configure(GPIOA, Pin::Pin7, PinConfig::OutputAF_50MHz_PushPull);
    // PA6 - SPI1_MISO
    gpio_pin_configure(GPIOA, Pin::Pin6, PinConfig::Input_PuPd);
    GPIOA->BSRR = GPIO_BSRR_BS6; // pull-up
    // PA5 - SPI1_CLK
    gpio_pin_configure(GPIOA, Pin::Pin5, PinConfig::OutputAF_50MHz_PushPull);
    // PA4 - SPI1_NSS
    gpio_pin_configure(GPIOA, Pin::Pin4, PinConfig::OutputAF_50MHz_PushPull);
    // Configure for SDC
    // CPOL = 0, CPHA = 0

    SPI1->CR1 = 0;
    SPI1->CR2 = 0;
    // Master mode select;
    SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_CRCEN | (0b111 << SPI_CR1_BR_Pos);
    SPI1->CR2 |= SPI_CR2_SSOE;

    spi_enable(SPI1);
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
    return spi_receive(spi);
}

void spi_send_buff(SPI_TypeDef *spi, uint8_t *data, size_t size)
{
    spi_send(spi, data[0]);
    for(size_t i = 1; i < size; i++) {
        spi_send(spi, data[i]);
    }
}

void spi_send_buff(SPI_TypeDef *spi, uint8_t *data, size_t size, void(*rx_callbak)(uint32_t) )
{
    spi_send(spi, data[0]);
    for(size_t i = 1; i < size; i++) {
        rx_callbak(spi_send_receive(spi, data[i]));
    }
}

void spi_wait_for_transfer_complete(SPI_TypeDef *spi)
{
    while((spi->SR & SPI_SR_BSY) != 0);
}

void sdc_switch_to_spi_mode()
{
    rcc_gpioa_enable();
    // PA7 - SPI1_MOSI
    gpio_pin_configure(GPIOA, Pin::Pin7, PinConfig::Output_50MHz_PushPull);
    // PA4 - SPI1_NSS
    gpio_pin_configure(GPIOA, Pin::Pin4, PinConfig::Output_50MHz_PushPull);
    // PA5 - SPI1_CLK
    gpio_pin_configure(GPIOA, Pin::Pin5, PinConfig::Output_50MHz_PushPull);

    // MOSI and NSS set to high
    GPIOA->BSRR |= GPIO_BSRR_BS7;
    GPIOA->BSRR |= GPIO_BSRR_BS4;

    for (int i = 0; i < 80; i++) {
        GPIOA->BSRR = GPIO_BSRR_BS5;
        delay(1);
        GPIOA->BSRR = GPIO_BSRR_BR5;
        delay(1);
    }
}

struct sdc_command {
    uint32_t start_bits :2;
    uint32_t command    :6;
    uint32_t argument   :32;
    uint32_t crc        :7;
    uint32_t stop_bit   :1;
} __attribute__((packed));

struct sdc_response {
    uint8_t idle_state  :1;
    uint8_t erase_reset :1;
    uint8_t illegal_command:1;
    uint8_t crc_error   :1;
    uint8_t erase_sequence_error:1;
    uint8_t adress_error:1;
    uint8_t param_error :1;
    uint8_t zero        :1;
} __attribute__((packed));

extern "C" int main()
{
    system_clock_setup();
    SysTick_Config(SystemCoreClock/1000);
    led_setup();
    usart1_setup();

    sdc_switch_to_spi_mode();
    spi1_setup();

    uint8_t data = 0;

    size_t sz_cmd = sizeof(sdc_command);

    sdc_command cmd;
    cmd.start_bits = 0b01;
    cmd.command = 1;
    cmd.argument = 0;
    cmd.crc = 0;
    cmd.stop_bit = 1;

    spi_enable(SPI1);

    spi_send(SPI1, 0x55);
    data = spi_receive(SPI1);
    spi_send(SPI1, 0x55);
    data = spi_receive(SPI1);
    spi_send(SPI1, 0x55);
    data = spi_receive(SPI1);
    spi_send(SPI1, 0x55);
    data = spi_receive(SPI1);
    spi_send(SPI1, 0x55);
    data = spi_receive(SPI1);


    spi_disable(SPI1);

    int x = 0;
    while (true) {
        led_on();
        usart_send(USART1, "Len on\n");
        delay(1000);
        led_off();
        usart_send(USART1, "Len off\n");
        delay(1000);
        x++;
    }

    return 0;
}