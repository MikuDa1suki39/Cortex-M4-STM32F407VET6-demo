#ifndef USART_H
#define USART_H

typedef void (*receivecallback_t)(uint8_t data);
typedef void (*sendcompletecallback_t)(void);

void usart_init(void);
void usart_send_data(uint8_t *data, uint16_t size);
void usart_send_data_async(uint8_t *data, uint16_t lenth);
void usart_send_string(const char *str);
void usart_receive_register(receivecallback_t callback);
void usart_sendcomplete_register(sendcompletecallback_t callback);
void USART1_IRQHandler(void);
void DMA2_Stream7_IRQHandler(void);

#endif // !
