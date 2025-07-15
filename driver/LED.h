#ifndef _LED_H
#define _LED_H

typedef struct led_desc
{
    uint32_t clk_source;
    GPIO_TypeDef *port;
    uint16_t pin;
    BitAction lvl_on;
    BitAction lvl_off;
} led_desc_t; // LED描述结构体
// static struct led_desc led0;上面写了 typedef led_desc_t就不用这么写了

void led_init(const led_desc_t *desc);

void led_deinit(const led_desc_t *desc);

void led_on(const led_desc_t *desc);

void led_off(const led_desc_t *desc);

#endif