#ifndef _DHT11_H
#define _DHT11_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#if defined(STM32F40_41xxx)

#include "stm32f4xx.h"

typedef GPIO_TypeDef *DHT11_GPIO_Port_t;

#else

#error dht11.h: No processor defined!

#endif

typedef struct dht11_t
{
    DHT11_GPIO_Port_t port; // 所使用的端口
    uint32_t pin;           // 所使用的引脚
    double temperature;     // 温度值
    double humidity;        // 湿度
    bool init;              // 初始化标志
} DHT11_t;

int dht11_init(DHT11_t *dht11, DHT11_GPIO_Port_t port, uint32_t pin); // 初始化DHT11
int dht11_readdata(DHT11_t *dht11);                                   // 获取DHT11的数据

#endif // ! _DHT11_H