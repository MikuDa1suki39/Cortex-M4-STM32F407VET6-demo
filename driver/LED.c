#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "stm32f4xx.h" // Device header

#include "LED.h"
#include "board.h"

void led_init(const led_desc_t *desc)
{
    RCC_AHB1PeriphClockCmd(desc->clk_source, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    memset(&GPIO_InitStructure, 0, sizeof(GPIO_InitTypeDef)); // 初始值置0
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = desc->pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Medium_Speed;

    GPIO_Init(desc->port, &GPIO_InitStructure);

    GPIO_WriteBit(desc->port, desc->pin, desc->lvl_off); // 先关闭LED灯，GPIO输出高电平
}

void led_deinit(const led_desc_t *desc)
{
    GPIO_WriteBit(desc->port, desc->pin, desc->lvl_off); // 先关闭LED灯，GPIO输出高电平
}

void led_on(const led_desc_t *desc)
{
    GPIO_WriteBit(desc->port, desc->pin, desc->lvl_on);
}

void led_off(const led_desc_t *desc)
{
    GPIO_WriteBit(desc->port, desc->pin, desc->lvl_off);
}

// #define LED0_PORT GPIOE
// #define LED0_RCC RCC_AHB1Periph_GPIOE
// #define LED0_PIN GPIO_Pin_5

// #define LED1_PORT GPIOE
// #define LED1_RCC RCC_AHB1Periph_GPIOE
// #define LED1_PIN GPIO_Pin_6

// #define LED2_PORT GPIOC
// #define LED2_RCC RCC_AHB1Periph_GPIOC
// #define LED2_PIN GPIO_Pin_13

// void led_init(void)
// {
//     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
//     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

//     GPIO_InitTypeDef GPIO_InitStructure;
//     memset(&GPIO_InitStructure, 0, sizeof(GPIO_InitTypeDef)); // 初始值置0

//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//     GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//     GPIO_InitStructure.GPIO_Pin = LED0_PIN | LED1_PIN | LED2_PIN;
//     GPIO_InitStructure.GPIO_Speed = GPIO_Medium_Speed;

//     GPIO_Init(LED0_PORT, &GPIO_InitStructure);
//     GPIO_Init(LED2_PORT, &GPIO_InitStructure);

//     GPIO_SetBits(GPIOE, LED0_PIN | LED1_PIN); // 先关闭LED灯，GPIO输出高电平
//     GPIO_SetBits(GPIOC, LED2_PIN);
// }

// void LED_ON(void)
// {
//     GPIO_ResetBits(GPIOE, GPIO_Pin_5);
// }

// void LED_OFF(void)
// {
//     GPIO_SetBits(GPIOE, GPIO_Pin_5);
// }
