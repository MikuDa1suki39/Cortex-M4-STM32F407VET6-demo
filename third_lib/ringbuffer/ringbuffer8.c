#include <stdlib.h>
#include <string.h>
#include "ringbuffer8.h"

#define rbb_len rb->length
// #define rbb_idx(x) (uint8_t *)rbb_buff + rbb_size *(x)
// #define dat_idx(d, x) (uint8_t *)(d) + rbb_size *(x)

struct ringbuffer8
{
    uint32_t tail;   // 尾指针，指向下一个要读取的位置
    uint32_t head;   // 头指针，指向下一个要写入的位置
    uint32_t length; // 数据缓冲区的长度

    uint8_t buffer[]; // 柔性数组，用于存储实际数据
};

ringbuffer8_t rb8_new(uint8_t *buff, uint32_t length)
{
    ringbuffer8_t rb = (ringbuffer8_t)buff;
    // rb->length = length - sizeof(struct ringbuffer8);
    rbb_len = length - sizeof(struct ringbuffer8);
    return rb;
}

static inline uint16_t next_head(ringbuffer8_t rb)
{
    return rb->head + 1 < rbb_len ? rb->head + 1 : 0;
}

static inline uint16_t next_tail(ringbuffer8_t rb)
{
    return rb->tail + 1 < rbb_len ? rb->tail + 1 : 0;
}

bool rb8_empty(ringbuffer8_t rb)
{
    return rb->head == rb->tail;
}

bool rb8_full(ringbuffer8_t rb)
{
    return next_head(rb) == rb->tail;
}

bool rb8_put(ringbuffer8_t rb, uint8_t data)
{
    if (next_head(rb) == rb->tail) // 检查缓冲区是否满
        return false;              // 满时返回失败

    rb->buffer[rb->head] = data; // 数据写入 head 指向的位置
    rb->head = next_head(rb);    // 更新 head 到下一个位置

    return true; // 返回成功
}

bool rb8_puts(ringbuffer8_t rb, uint8_t *data, uint32_t size)
{
    bool ret = true; // 初始成功状态

    for (uint16_t i = 0; i < size && ret; i++) // 遍历每个字节
    {
        ret = rb8_put(rb, data[i]); // 调用 rb8_put 写入单字节
    } // 若 rb8_put 失败，终止循环

    return ret; // 返回整体结果
}

bool rb8_get(ringbuffer8_t rb, uint8_t *data)
{
    if (rb->head == rb->tail)
        return false;

    *data = rb->buffer[rb->tail];
    rb->tail = next_tail(rb);

    return true;
}

bool rb8_gets(ringbuffer8_t rb, uint8_t *data, uint32_t size)
{
    bool ret = true;

    for (uint16_t i = 0; i < size && ret; i++)
    {
        ret = rb8_get(rb, &data[i]);
    }

    return ret;
}
