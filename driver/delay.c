#include <stdint.h>
#include "stm32f4xx.h" // Device header

void delay_us(uint32_t us)
{
    SysTick->LOAD = us * (SystemCoreClock / 1000000) - 1;                 // 1us
    SysTick->VAL = 0;                                                     // Clear current value register
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk; // Start timer
    while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0)
        ;              // Wait until the timer reaches 0
    SysTick->CTRL = 0; // Stop timer
}

void delay_ms(uint32_t ms)
{
    while (ms--)
    {
        delay_us(1000); // Call delay_us function to create 1ms delay
    }
}