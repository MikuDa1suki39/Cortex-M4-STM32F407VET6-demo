#include "main.h"

// static uint8_t rx_buffer[128];

// static ringbuffer8_t rxringbuffer;

// static volatile uint8_t rxdata;

// static void rx_receive(uint8_t data)
// {
// 	// rxdata = data;
// 	if (!rb8_full(rxringbuffer))
// 	{
// 		rb8_put(rxringbuffer, data);
// 	}
// }

/**
 * EasyLogger demo
 */
static void test_elog(void)
{
	/* test log output for all level */
	log_a("Hello EasyLogger!");
	log_e("Hello EasyLogger!");
	log_w("Hello EasyLogger!");
	log_i("Hello EasyLogger!");
	log_d("Hello EasyLogger!");
	log_v("Hello EasyLogger!");
	elog_raw("Hello EasyLogger!");
}

int main(void)
{
	mem_init();

	board_init();
	usart_init();
	// usart_receive_register(rx_receive);
	led_init(&led0);

	/* initialize EasyLogger */
	elog_init();
	/* set EasyLogger log format */
	elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
	elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_T_INFO | ELOG_FMT_P_INFO));
	elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_T_INFO | ELOG_FMT_P_INFO));
	/* start EasyLogger */
	elog_start();

	// rxringbuffer = rb8_new(rx_buffer, sizeof(rx_buffer));
	// uint8_t rx_data;

	while (1)
	{
		// if (!rb8_empty(rxringbuffer))
		// {
		// 	rb8_get(rxringbuffer, &rx_data);
		// }

		/* test logger output */
		test_elog();
		delay_ms(2000);
	}
}