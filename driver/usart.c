#include "stdint.h"
#include "stdbool.h"
#include "string.h"

#include <stm32f4xx.h>

#include "usart.h"

static receivecallback_t receive_callback;
static sendcompletecallback_t sendcomplete_callback;

static void usart_gpio(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    memset(&GPIO_InitStructure, 0, sizeof(GPIO_InitTypeDef));

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; // TX and RX pins
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);  // TX
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1); // RX
}

static void usart_usart(void)
{
    USART_InitTypeDef USART_InitStructure;
    memset(&USART_InitStructure, 0, sizeof(USART_InitTypeDef));
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; // Enable TX and RX
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

    USART_Init(USART1, &USART_InitStructure);
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
    USART_Cmd(USART1, ENABLE);
}

static void usart_nvic(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    memset(&NVIC_InitStructure, 0, sizeof(NVIC_InitTypeDef));
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);

    memset(&NVIC_InitStructure, 0, sizeof(NVIC_InitTypeDef));
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

    NVIC_Init(&NVIC_InitStructure);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

void usart_init(void)
{
    usart_gpio();
    usart_usart();
    usart_nvic();
}

void usart_send_data(uint8_t *data, uint16_t lenth)
{
    for (uint16_t i = 0; i < lenth; i++)
    {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
            ;
        USART_SendData(USART1, data[i]);
    }
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
        ;
}

void usart_send_data_async(uint8_t *data, uint16_t lenth)
{
    DMA_DeInit(DMA2_Stream7);

    DMA_InitTypeDef DMA_InitStructure;
    memset(&DMA_InitStructure, 0, sizeof(DMA_InitTypeDef));
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;                          // USART1的TX是通道4里的7数据流
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;       // 外设地址为USART1数据寄存器
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)data;                 // 内存地址为NULL，实际使用时需要设置
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                 // 数据传输方向为内存到外设
    DMA_InitStructure.DMA_BufferSize = lenth;                               // 数据缓冲区大小，实际使用时需要设置
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // 外设地址不自增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // 内存地址自增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据大小为字节
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // 内存数据大小为字节
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // DMA工作模式为普通模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                   // DMA优先级为中等
    // DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;                   // FIFO模式
    // DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;   // FIFO阈值为1/4满
    // DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;             // 内存突发传输为单次传输
    // DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;     // 外设突发传输为单次传输

    DMA_Init(DMA2_Stream7, &DMA_InitStructure);    // USART1的TX使用DMA2的Stream7
    DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE); // 使能DMA传输完成中断
    DMA_Cmd(DMA2_Stream7, ENABLE);
}

void usart_send_string(const char *str)
{
    uint16_t len = strlen(str);
    usart_send_data((uint8_t *)str, len);
}

void usart_receive_register(receivecallback_t callback)
{
    receive_callback = callback;
}

void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        uint8_t data = USART_ReceiveData(USART1);
        if (receive_callback)
        {
            receive_callback(data);
        }
        USART_ClearITPendingBit(USART1, USART_IT_RXNE); // Clear the interrupt flag
    }
}

void usart_sendcomplete_register(sendcompletecallback_t callback)
{
    sendcomplete_callback = callback;
}

void DMA2_Stream7_IRQHandler(void)
{
    if (DMA_GetFlagStatus(DMA2_Stream7, DMA_FLAG_TCIF7) != RESET)
    {
        if (sendcomplete_callback)
        {
            sendcomplete_callback();
        }
        DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TCIF7);
    }
}