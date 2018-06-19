#include <stdlib.h>
#include <stdint.h>
#include <type_traits>
#include "stm32f1xx.h"

#include "gpio.h"
#include "rcc.h"
#include "usart.h"
#include "spi.h"

#include "board.h"
#include "sdcard.h"

uint32_t apb1clk = 0;
uint32_t apb2clk = 0;
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

void system_clock_setup(void)
{
    rcc_hse_enable_and_wait_for_ready();
    rcc_css_enable();

    rcc_pll_configure(PLLMUL::Mul_9, PLLXTPRE::HSE_div1, PLLSRC::HSE);
    rcc_pll_enable_and_wait_for_ready();

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

    rcc_sysclk_source_select(SYSCLK_SRC::PLL);

    SystemCoreClockUpdate();
    apb1clk = SystemCoreClock/2;
    apb2clk = SystemCoreClock;
}

void led_setup()
{
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

void spi1_setup()
{
    rcc_gpioa_enable();
    rcc_spi1_enable();

    gpio_pin_configure(SDC_PORT, SDC_MOSI, PinConfig::OutputAF_50MHz_PushPull);
    gpio_pin_configure(SDC_PORT, SDC_MISO, PinConfig::Input_PuPd);
    gpio_pin_pullup(SDC_PORT, PinFlag::Pin_6);
    gpio_pin_configure(SDC_PORT, SDC_CLK, PinConfig::OutputAF_50MHz_PushPull);
    gpio_pin_configure(SDC_PORT, SDC_SS, PinConfig::OutputAF_50MHz_PushPull);


    // Configure for SDC
    // CPOL = 0, CPHA = 0
    SPI1->CR1 = 0;
    SPI1->CR2 = 0;
    // Master mode select;
    SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_CRCEN | (0b111 << SPI_CR1_BR_Pos);
    SPI1->CR2 |= SPI_CR2_SSOE;

    spi_enable(SPI1);
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

extern "C" int main()
{
    system_clock_setup();
    SysTick_Config(SystemCoreClock/1000);
    led_setup();
    usart1_setup();

    sdc_switch_to_spi_mode();
    spi1_setup();

    uint8_t data = 0;

    sdc_command cmd;
    cmd.start_bits = 0b01;
    cmd.command = 0;
    cmd.argument = 0;
    cmd.crc = 0b1001010;
    cmd.crc = 0x00;
    cmd.stop_bit = 1;

    spi_enable(SPI1);

    sdc_send_command(SPI1, cmd);

    cmd.start_bits = 0b01;
    cmd.command = 8;
    cmd.argument = 0x1AA;
    cmd.crc = 0b0000111;

    sdc_send_command(SPI1, cmd);

    cmd.start_bits = 0b01;
    cmd.command = 58;
    cmd.argument = 0x00;
    cmd.crc = 0b0111010;

    sdc_send_command(SPI1, cmd);

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