#include "main.h"

static uint8_t rxdata[16];
static uint16_t rxlen;
bool dmastate;

static void timeelapse(void)
{
	led_toggle(&led0);
}

static void usart_send()
{
	dmastate = true;
}

static void usart_receive(uint8_t data)
{
	if (rxlen < 15)
	{
		rxdata[rxlen++] = data;
	}
}

int main(void)
{
	board_init();

	usart_init();
	led_init(&led0);
	timer_init(1000 * 250);
	time_elapsed_callback_register(timeelapse);
	usart_receive_register(usart_receive);
	usart_sendcomplete_register(usart_send);

	char str[64];
	while (1)
	{
		if (rxlen > 0)
		{
			dmastate = false;
			usart_send_string("receive:");
			usart_send_data(rxdata, rxlen);
			rxlen = 0;
			usart_send_string("\r\n");
			strcpy(str, "hello");
			while (rxlen == 0)
			{
				usart_send_data_async((uint8_t *)str, strlen(str));
				delay_ms(250);
			}

			while (!(dmastate))
				;
		}
	}
}