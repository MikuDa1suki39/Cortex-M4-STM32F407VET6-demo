#include "main.h"

char show_buffer[100];

DHT11_t dht11;

int main(void)
{
	board_init();
	usart_init();
	led_init(&led0);

	usart_send_string("DHT11 start\r\n");

	LCD_Init();
	LCD_Clear(RED);
	delay_ms(1000);
	LCD_Clear(BLUE);
	delay_ms(1000);

	int dev = dht11_init(&dht11, GPIOA, GPIO_Pin_6);
	printf("Device:%d\r\n",dev);

	while (1)
	{
		led_on(&led0);
		dht11_readdata(&dht11);
		memset(show_buffer, 0, sizeof(show_buffer));
		sprintf(show_buffer, "T: %0.2f   H: %0.2f\r\n", dht11.temperature, dht11.humidity);
		printf("%s", show_buffer);
		LCD_ShowString(0, 0, 240, 30, 24, show_buffer);
		delay_ms(1000);
	}
}