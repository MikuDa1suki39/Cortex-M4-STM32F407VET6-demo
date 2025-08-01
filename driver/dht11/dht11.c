#include "dht11.h"
#include "delay.h"

#define DHT11_DelayMs(t) delay_ms(t)
#define DHT11_DelayUs(t) delay_us(t)

#define DHT11_PIN_HIGH 1
#define DHT11_PIN_LOW 0

#if defined(STM32F40_41xxx)

#include "stm32f4xx.h"

#define __DHT11_CONFIG_IO_INPUT(DHT11)                    \
    {                                                     \
        GPIO_InitTypeDef GPIO_InitStructure;              \
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;      \
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      \
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; \
        GPIO_InitStructure.GPIO_Pin = DHT11->pin;         \
        GPIO_Init(DHT11->port, &GPIO_InitStructure);      \
    }

#define __DHT11_CONFIG_IO_OUTPUT(DHT11)                   \
    {                                                     \
        GPIO_InitTypeDef GPIO_InitStructure;              \
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;     \
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    \
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      \
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; \
        GPIO_InitStructure.GPIO_Pin = DHT11->pin;         \
        GPIO_Init(DHT11->port, &GPIO_InitStructure);      \
    }

#define __DHT11_IO_SET(DHT11, value)                 \
    {                                                \
        if (value == DHT11_PIN_HIGH)                 \
            GPIO_SetBits(DHT11->port, DHT11->pin);   \
        else                                         \
            GPIO_ResetBits(DHT11->port, DHT11->pin); \
    }

#define DHT11_IO_H(DHT11) {__DHT11_IO_SET(DHT11, DHT11_PIN_HIGH)}
#define DHT11_IO_L(DHT11) {__DHT11_IO_SET(DHT11, DHT11_PIN_LOW)}
#define DHT11_IO_IN(DHT11) GPIO_ReadInputDataBit(DHT11->port, DHT11->pin)

#endif

/******************************************************************************
 * @brief      复位DHT11
 *
 * @param[in]  dht11  :  dht11结构体指针
 *
 * @return     0, 表示正常, 其他值表示失败
 *
 ******************************************************************************/
static int dht11_rst(DHT11_t *dht11)
{
    __DHT11_CONFIG_IO_OUTPUT(dht11);
    DHT11_IO_L(dht11);
    DHT11_DelayMs(20);
    DHT11_IO_H(dht11);
    DHT11_DelayUs(30);
    __DHT11_CONFIG_IO_INPUT(dht11);

    return 0;
}

/******************************************************************************
 * @brief      等待DHT11的回应
 *
 * @param[in]  dht11  :  dht11结构体指针
 *
 * @return     0, 存在, 返回1:未检测到DHT11的存在
 *
 ******************************************************************************/
static int dht11_rsp(DHT11_t *dht11)
{
    int ret = 0;
    while (DHT11_IO_IN(dht11) && ret < 100)
    {
        ret++;
        DHT11_DelayUs(1);
    }

    if (ret >= 100)
    {
        return -2;
    }
    else
    {
        ret = 0;
    }

    while (!DHT11_IO_IN(dht11) && ret < 100)
    {
        ret++;
        DHT11_DelayUs(1);
    }

    if (ret >= 100)
    {
        return -3;
    }

    return 0;
}

/******************************************************************************
 * @brief      从DHT11读取一个位
 *
 * @param[in]  dht11  :  dht11结构体指针
 *
 * @return     0, 1
 *
 ******************************************************************************/
static uint8_t dht11_readbit(DHT11_t *dht11)
{
    int ret = 0;
    while (DHT11_IO_IN(dht11) && ret < 100)
    {
        ret++;
        DHT11_DelayUs(1);
    }

    ret = 0;
    while (!DHT11_IO_IN(dht11) && ret < 100)
    {
        ret++;
        DHT11_DelayUs(1);
    }

    DHT11_DelayUs(40);
    if (DHT11_IO_IN(dht11))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/******************************************************************************
 * @brief      从DHT11读取一个字节
 *
 * @param[in]  dht11  :  dht11结构体指针
 *
 * @return     读到的数据
 *
 ******************************************************************************/
static uint8_t dht11_readbyte(DHT11_t *dht11)
{
    uint8_t dat, i;
    dat = 0;
    for (i = 0; i < 8; i++)
    {
        dat <<= 1;
        dat |= dht11_readbit(dht11);
    }
    return dat;
}

/******************************************************************************
 * @brief      从DHT11读取一次数据
 *                   temp:温度值(范围:0~50°)
 *                   humi:湿度值(范围:20%~90%)
 *
 * @param[in]  dht11  :  dht11结构体指针
 *
 * @return     0, 表示正常, 其他值表示失败
 *
 ******************************************************************************/
int dht11_readdata(DHT11_t *dht11)
{
    if (!dht11 || !dht11->init)
        return -1; // 初始化引脚之后

    uint8_t buff[5], i;

    dht11_rst(dht11);

    if (dht11_rsp(dht11) == 0)
    {
        for (i = 0; i < 5; i++)
        {
            buff[i] = dht11_readbyte(dht11);
        }
        if (buff[0] + buff[1] + buff[2] + buff[3] == buff[4])
        {
            dht11->humidity = buff[0];
            dht11->temperature = buff[2];
        }
    }
    else
    {
        return -2;
    }

    return 0;
}

/******************************************************************************
 * @brief      初始化DHT11的IO口 DQ 同时检测DHT11的存在.
 *
 * @param[in]  dht11  :  dht11结构体指针
 *
 * @return     0, 表示正常, 其他值表示失败
 *
 ******************************************************************************/
int dht11_init(DHT11_t *dht11, DHT11_GPIO_Port_t port, uint32_t pin)
{
    if (!dht11)
        return -1;

        // 配置引脚，默认输出
#if defined(STM32F40_41xxx)

    assert_param(IS_GPIO_ALL_PERIPH(port));

    if (port == GPIOA)
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    }
    else if (port == GPIOB)
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    }
    else if (port == GPIOC)
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    }
    else if (port == GPIOD)
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    }
    else if (port == GPIOE)
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    }
    else if (port == GPIOF)
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
    }
    else if (port == GPIOG)
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
    }
    else
        return -1;

    __DHT11_CONFIG_IO_OUTPUT(dht11);

#endif

    dht11->port = port;
    dht11->pin = pin;
    dht11->temperature = 0.0;
    dht11->humidity = 0.0;
    dht11->init = true;

    DHT11_IO_H(dht11);
    DHT11_DelayUs(30);
    dht11_rst(dht11);

    return dht11_rsp(dht11);
}