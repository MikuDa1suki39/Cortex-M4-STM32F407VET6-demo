#include <string.h>
#include <stdbool.h>

#include "stm32f4xx.h" // Device header

#include "led.h"


void led_init(const led_t *led)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitTypeDef));
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = led->pin;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(led->port, &GPIO_InitStruct);
    GPIO_SetBits(led->port, led->pin); // Set the pin to high (LED off)
}

void led_on(const led_t *led)
{
    GPIO_WriteBit(led->port, led->pin, led->on);
}

void led_off(const led_t *led)
{
    GPIO_WriteBit(led->port, led->pin,led->off);
}

void led_toggle(const led_t *led)
{
    uint8_t current_state = GPIO_ReadOutputDataBit(led->port, led->pin);
    if (current_state == (uint8_t)led->on)
    {
        GPIO_WriteBit(led->port, led->pin, led->off);
    }
    else
    {
        GPIO_WriteBit(led->port, led->pin, led->on);
    }
}

