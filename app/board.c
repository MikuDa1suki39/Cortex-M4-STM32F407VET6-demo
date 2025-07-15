#include "stm32f4xx.h"
#include "LED.h"

const led_desc_t led0 =
    {
        .clk_source = RCC_AHB1Periph_GPIOE,
        .port = GPIOE,
        .pin = GPIO_Pin_5,
        .lvl_on = Bit_RESET,
        .lvl_off = Bit_SET,
};

const led_desc_t led1 =
    {
        .clk_source = RCC_AHB1Periph_GPIOE,
        .port = GPIOE,
        .pin = GPIO_Pin_6,
        .lvl_on = Bit_RESET,
        .lvl_off = Bit_SET,
};

const led_desc_t led2 =
    {
        .clk_source = RCC_AHB1Periph_GPIOC,
        .port = GPIOC,
        .pin = GPIO_Pin_13,
        .lvl_on = Bit_RESET,
        .lvl_off = Bit_SET,
};