#include "main.h"

static Shell shell;

static uint8_t rx_buffer[128];

static ringbuffer8_t rxringbuffer;

static volatile uint8_t rxdata;

static char shell_Buffer[512];

static void rx_receive(uint8_t data)
{
	// rxdata = data;
	if (!rb8_full(rxringbuffer))
	{
		rb8_put(rxringbuffer, data);
	}
}

static signed short shell_write_(char *data, unsigned short len)
{
	usart_send_data((uint8_t *)data, len);
	return len;
}

int main(void)
{
	mem_init();
	board_init();
	usart_init();
	usart_receive_register(rx_receive);

	rxringbuffer = rb8_new(rx_buffer, sizeof(rx_buffer));

	shell.write = shell_write_;
	shellInit(&shell, shell_Buffer, sizeof(shell_Buffer));

	uint8_t rx_data;

	while (1)
	{
		if (!rb8_empty(rxringbuffer))
		{
			rb8_get(rxringbuffer, &rx_data);
			shellHandler(&shell, rx_data);
		}

		// if (rxdata != 0)
		// {
		// 	// shellTask(&shell);
		// 	shellHandler(&shell, rxdata);
		// 	rxdata = 0;
		// }
	}
}