#include "main.h"

int main(void)
{
    led_init(&led0);
    led_init(&led1);
    led_init(&led2);

    while (1)
    {
        led_on(&led0);
        delay_ms(500); // Delay for 1 second
        led_off(&led0);
        delay_ms(500); // Delay for 1 second
        led_on(&led1);
        delay_ms(500); // Delay for 1 second
        led_off(&led1);
        delay_ms(500); // Delay for 1 second
        led_on(&led2);
        delay_ms(500); // Delay for 1 second
        led_off(&led2);
        delay_ms(500); // Delay for 1 second
    }
}