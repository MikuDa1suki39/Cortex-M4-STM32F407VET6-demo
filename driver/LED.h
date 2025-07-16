#ifndef LED_H
#define LED_H

#include <stdbool.h>

typedef struct led_desc
{
    uint32_t clk;
    GPIO_TypeDef *port;
    uint16_t pin;
    BitAction on;
    BitAction off;
} led_t;

void led_init(const led_t *led);

void led_on(const led_t *led);

void led_off(const led_t *led);

void led_toggle(const led_t *led);

#endif //