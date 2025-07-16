#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

typedef void (*time_elapsed_callback_t)(void);

void timer_init(uint32_t us);
void time_elapsed_callback_register(time_elapsed_callback_t callback);
void TIM2_IRQHandler(void);
#endif // !TIMER_H
