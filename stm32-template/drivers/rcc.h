#ifndef __RCC_H
#define __RCC_H

#include "stm32f1xx.h"

void rcc_hse_enable();
void rcc_wait_for_hse_ready();
void rcc_hse_enable_and_wait_for_ready();
void rcc_css_enable();

void rcc_pll_enable();
void rcc_pll_disable();
void rcc_wait_for_pll_ready();
void rcc_pll_enable_and_wait_for_ready();

enum class PLLMUL {
    Mul_2 , Mul_3, Mul_4, Mul_5, Mul_6, Mul_7, Mul_8, Mul_9,
    Mul_10, Mul_11, Mul_12, Mul_13, Mul_14, Mull_15, Mul_16,
};
enum class PLLXTPRE { HSE_div1, HSE_div2};
enum class PLLSRC { HSI_div2, HSE};
void rcc_pll_configure(PLLMUL mull, PLLXTPRE xtpre, PLLSRC clk_src);

enum class  SYSCLK_SRC { HSI, HSE, PLL};
void rcc_sysclk_source_select(SYSCLK_SRC clk_src);

void rcc_gpioa_enable();
void rcc_gpioc_enable();
void rcc_usart1_enable();
void rcc_spi1_enable();


#endif // __RCC_H