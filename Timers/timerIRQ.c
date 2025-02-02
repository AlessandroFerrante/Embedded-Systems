/**
 * @file timerIRQ.c
 * @author Alessandro Ferrante (github@alessandroferrante.net)
 * @brief 
 * @version 0.1
 * @date 2024-10-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "stm32_unict_lib.h"
#include <stdio.h>

void setup(void)
{
    ClockConfig();
	DISPLAY_init();
	GPIO_init(GPIOB);
	GPIO_config_output(GPIOB, 0);
	TIM_init(TIM2);
	TIM_config_timebase(TIM2, 42000, 2000);
    // increment frequency = 84e6 / 42e3 = 2000 Hz
    // autoreload = 2000 --> update every second
	TIM_enable_irq(TIM2, IRQ_UPDATE);
	TIM_on(TIM2);
}

void TIM2_IRQHandler(void)
{
	if (TIM_update_check(TIM2)) {
        // update every second
		GPIO_toggle(GPIOB,0);
		TIM_update_clear(TIM2);
	}
}

void loop(void)
{
	char s[6];
	sprintf(s, "%4d", TIM_get(TIM2));
	DISPLAY_puts(0,s);
}

int main()
{
	setup();
	for(;;) {
		loop();
	}
	return 0;
}