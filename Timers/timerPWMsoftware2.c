/**
 * @file timerPWMsoftware2.c
 * @author Alessandro Ferrante (github@alessandroferrante.net)
 * @brief This code implements a software PWM using a timer interrupt, to vary the brightness of a LED.
 * 
 * The code configures GPIO pins and external interrupts to control the duty cycle of a PWM signal.
 * The PWM signal is generated on pin PB0 with timer (TIM2), and its duty cycle can be adjusted 
 * using buttons connected to pins PB4 (button Y) and PB10 (button X).
 * 
 * ? New implementations:
 * - The current duty cycle is displayed on a 7-segment display.
 * - Additional functionality to turn the PWM signal on and off using a button (Z) with the variable `LED_on`.

 * @version 0.1
 * @date 2024-10-18
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "stm32_unict_lib.h"

int T_on, T_off;

int LED_on = 0;

#define PERIOD 2000

void setup(void)
{
    ClockConfig();
    // configure the clock @ 84 MHz
    DISPLAY_init();
    GPIO_init(GPIOB); // initialize port B
    GPIO_init(GPIOC); // initialize port C
    GPIO_config_output(GPIOB, 0); // configure pin PB0 as output
    GPIO_config_output(GPIOC, 2); // configure pin PC2 as output

    GPIO_config_input(GPIOB, 10); // configure pin PB10 as input
    GPIO_config_input(GPIOB, 4); // configure pin PB4 as input
    GPIO_config_input(GPIOB, 5); // configure pin PB5 as input
    GPIO_config_input(GPIOB, 6); // configure pin PB6 as input
   
    GPIO_config_EXTI(GPIOB, EXTI10); // attach PB10 to EXTI10
    GPIO_config_EXTI(GPIOB, EXTI4); // attach PB4 to EXTI4
    GPIO_config_EXTI(GPIOB, EXTI5); // attach PB5 to EXTI5
    GPIO_config_EXTI(GPIOB, EXTI6); // attach PB6 to EXTI6

    EXTI_enable(EXTI10, FALLING_EDGE);
    EXTI_enable(EXTI4, FALLING_EDGE);
    EXTI_enable(EXTI5, FALLING_EDGE);
    EXTI_enable(EXTI6, FALLING_EDGE);

    T_on = PERIOD / 2;
    T_off = PERIOD / 2;

	TIM_init(TIM2);
	TIM_config_timebase(TIM2, 420, T_off);
	TIM_enable_irq(TIM2, IRQ_UPDATE);
	TIM_on(TIM2);

}
/**
 * @brief 
 * - if LED_on is true:
 * alternates the status of the pin GPIOB 0,
 * "creating" a PWM signal on pin GPIOB 0 and
 * updating a period that alternates between T_on and T_off,
 * is used to control the brightness of the LED;
 * - if LED_on is false:
 * set the GPIOB 0 Pin at low;
 */
void TIM2_IRQHandler(void)
{
	if (TIM_update_check(TIM2)) {
        if (LED_on)
		    GPIO_toggle(GPIOB,0);
        else
		    GPIO_write(GPIOB,0, 0);
        if (GPIO_read(GPIOB,0) == 1) {
            // stato on
            TIM2->ARR = T_on;
        }        
        else {
            // stato off
            TIM2->ARR = T_off;
        }
		TIM_update_clear(TIM2);
	}
}

/**
 * @brief Manages the interrupt (of the X button),
 * If T_on is greater than 20 and LED_on is true, 
 * decrement of 10 units the value of T_on, and update T_off.
 */
void EXTI15_10_IRQHandler(void) {   
    if (EXTI_isset(EXTI10)) {       
        // X button, decrement T_on
        if (T_on > 20 && LED_on) {
            T_on -= 10;
            T_off = PERIOD - T_on;
        }
         EXTI_clear(EXTI10);
    }
}

/**
 * @brief Manages the interrupt (of the Y button),
 * If T_on is less than PERIOD - 20 and LED_on is true, 
 * increment of 10 units the value of T_on, and update T_off.
 */ 
void EXTI4_IRQHandler(void) {   
    if (EXTI_isset(EXTI4)) {
        // Y button, increment T_on
        if (T_on < PERIOD - 20 && LED_on) {
            T_on += 10;
            T_off = PERIOD - T_on;
        }
        EXTI_clear(EXTI4);
    }
}

void EXTI9_5_IRQHandler(void) {
    if (EXTI_isset(EXTI5)) {
        // Z on/off the PWM signal
        LED_on = !LED_on;
        EXTI_clear(EXTI5);
    }
    if (EXTI_isset(EXTI6)) {
        EXTI_clear(EXTI6);
    }
}

// current duty cycle displayed on a display.
void loop(void) {
    char s[5];
    sprintf(s, "%4d", T_on);
    DISPLAY_puts(0, s);   
}

int main() {
    setup();
    for (;;) {
        loop();
    }
}