#include "rcc.h"

void rcc_hse_enable()
{
    RCC->CR |= RCC_CR_HSEON;
}

void rcc_wait_for_hse_ready()
{
    while ((RCC->CR & RCC_CR_HSERDY) == 0);
}

void rcc_hse_enable_and_wait_for_ready()
{
    RCC->CR |= RCC_CR_HSEON;
    while ((RCC->CR & RCC_CR_HSERDY) == 0);
}

void rcc_css_enable()
{
    RCC->CR |= RCC_CR_CSSON;
}

void rcc_pll_enable()
{
    RCC->CR |= RCC_CR_PLLON;
}

void rcc_pll_disable()
{
    RCC->CR &= ~RCC_CR_PLLON;
}

void rcc_wait_for_pll_ready()
{
    while ((RCC->CR & RCC_CR_PLLRDY) == 0);
}

void rcc_pll_enable_and_wait_for_ready()
{
    RCC->CR |= RCC_CR_PLLON;
    while ((RCC->CR & RCC_CR_PLLRDY) == 0);
}

void rcc_pll_configure(PLLMUL mull, PLLXTPRE xtpre, PLLSRC clk_src)
{
    uint32_t pll_config = (static_cast<uint32_t>(mull) << RCC_CFGR_PLLMULL_Pos) |
                          (static_cast<uint32_t>(xtpre) << RCC_CFGR_PLLXTPRE_Pos) |
                          (static_cast<uint32_t>(clk_src) << RCC_CFGR_PLLSRC_Pos);
    
    rcc_pll_disable();
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC_Msk|RCC_CFGR_PLLXTPRE_Msk|RCC_CFGR_PLLMULL_Msk);
    RCC->CFGR |= pll_config;
}

void rcc_sysclk_source_select(SYSCLK_SRC clk_src)
{
    RCC->CFGR &= (~RCC_CFGR_SW);
    RCC->CFGR |= static_cast<uint32_t>(clk_src);

    // wait of clk switch
    while ((RCC->CFGR & RCC_CFGR_SWS) >> RCC_CFGR_SWS_Pos != static_cast<uint32_t>(clk_src));
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