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
   //gpio_pin_configure(SDC_PORT, SDC_CS, PinConfig::OutputAF_50MHz_PushPull);

    // Configure for SDC
    // CPOL = 0, CPHA = 0
    SPI1->CR1 = 0;
    SPI1->CR2 = 0;
    // Master mode select;
    SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_SSM | (0b111 << SPI_CR1_BR_Pos);
    SPI1->CR2 |= SPI_CR2_SSOE;

    spi_enable(SPI1);
}

extern "C" int main()
{
    system_clock_setup();
    SysTick_Config(SystemCoreClock/1000);
    usart1_setup();

    crc7_generate_table();
    spi1_setup();

    rcc_gpioc_enable();
    GpioPin led_pin(GPIOC, Pin::Pin13, PinConfig::Output_2MHz_PushPull);

    delay(1);
    SDCard sd_card(SPI1, SDC_PORT, SDC_CS);
    sd_card.Initialize();

    int x = 0;
    while (true) {

        led_pin.Low();
        usart_send(USART1, "Len on\n");
        delay(1000);
        led_pin.High();
        usart_send(USART1, "Len off\n");
        delay(1000);

        x++;
    }

    return 0;
}