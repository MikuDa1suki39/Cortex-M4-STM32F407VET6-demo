#include "main.h"

static Shell shell;

static volatile uint8_t rxdata;

static char shell_Buffer[512];

static void rx_receive(uint8_t data)
{
	rxdata = data;
}

static signed short shell_read_(char *data, unsigned short len)
{
	if (rxdata != 0)
	{
		*data = rxdata;
		rxdata = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}

static signed short shell_write_(char *data, unsigned short len)
{
	usart_send_data((uint8_t*)data,len);
	return len;
}


int main(void)
{
	board_init();
	usart_init();
	usart_receive_register(rx_receive);

	shell.read = shell_read_;
	shell.write = shell_write_;
	shellInit(&shell, shell_Buffer, sizeof(shell_Buffer));

	while (1)
	{
		// OsTaskCreate(shellTask, &shell, );
		shellTask(&shell);
	}
}