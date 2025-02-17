/**
 * @file timerPWMhardware.c
 * @author Alessandro Ferrante (github@alessandroferrante.net)
 * @brief This file contains the implementation of PWM hardware timers.
 * 
 * The alternate function configuration is used to set specific pins to operate
 * in alternate modes, such as PWM output, rather than their default GPIO mode.
 * This allows the microcontroller to use the pins for specialized functions
 * like timer channels, communication interfaces, etc.
 * 
 * Alternate functions are essential for utilizing the full capabilities of 
 * microcontroller peripherals. They enable efficient use of hardware resources 
 * by multiplexing pins for different functionalities.
 * 
 * This example configures TIM3 to generate a PWM signal on pin PB0. The duty 
 * cycle of the PWM signal can be adjusted using external interrupts on pins 
 * PB4 and PB10, while PB5 is used to toggle the PWM output on and off.
 * 
 * PERIOD
 * The period defines the total duration of a PWM cycle. In other words, 
 * it is the total number of timer counts needed to complete a PWM signal cycle. 
 * In the example, the period is set to 2000. This means that the timer will count 
 * up to 2000 before reset and start over. The PWM signal frequency is determined by 
 * the frequency of the timer clock divided by the value of the period.
 * 
 * Duty Cycle
 * The duty cycle represents the proportion of the period in which the 
 * PWM signal is high (active). It is controlled by the T_ON variable. 
 * If T_on is 1000 and the period is 2000, the duty cycle will be 50%, 
 * since the signal will be halfway up to the total period.
 * Changing T_on the width of the PWM pulse is adjusted, effectively 
 * controlling the duty cycle.
 * 
 * @version 0.1
 * @date 2025-02-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */

 #include "stm32_unict_lib.h"
 #include <stdio.h>
 int T_on;
 
 int LED_on = 0;
 
 #define PERIOD 2000
 
 void setup(void)
 {
     ClockConfig();
     // configure the clock @ 84 MHz
     DISPLAY_init();
     GPIO_init(GPIOB); // initialize port B
     GPIO_init(GPIOC); // initialize port C
     GPIO_config_alternate(GPIOB, 0, 2); // configure pin PB0 as CH3 of TIM3
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
 
     TIM_init(TIM3);
     TIM_config_timebase(TIM3, 420, PERIOD);
     TIM_config_PWM(TIM3, 3); // configure CH3 of TIM3 as PWM
     TIM3->CCR3 = 0;
     TIM_on(TIM3);
 
     ADC_init(ADC1, ADC_RES_10, ADC_ALIGN_RIGHT); // ADC1, 8bit, 
     ADC_channel_config(ADC1, GPIOC, 0, 10);
     ADC_on(ADC1);
     ADC_sample_channel(ADC1, 10);
 }
 
 void EXTI15_10_IRQHandler(void)
 {
     if (EXTI_isset(EXTI10)) {       
         // X button, decrement T_on
         if (T_on > 20 && LED_on) {
             T_on -= 10;
             TIM3->CCR3 = T_on;
         }
          EXTI_clear(EXTI10);
     }
 }
 
 void EXTI4_IRQHandler(void)
 {
     if (EXTI_isset(EXTI4)) {
         // Y button, increment T_on
         if (T_on < PERIOD - 20 && LED_on) {
             T_on += 10;
             TIM3->CCR3 = T_on;
         }
         EXTI_clear(EXTI4);
     }
 }
 
 void EXTI9_5_IRQHandler(void)
 {
     if (EXTI_isset(EXTI5)) {
         // Z on/off
         LED_on = !LED_on;
         if (LED_on) {
             TIM3->CCR3 = T_on;
         }
         else {
             TIM3->CCR3 = 0;
         }
         EXTI_clear(EXTI5);
     }
     if (EXTI_isset(EXTI6)) {
         ADC_sample_channel(ADC1, 10);
         ADC_start(ADC1);
         while (!ADC_completed(ADC1)){ }
         int value = ADC_read(ADC1);
         int k = (value * 1000) / 1023 + 5;
         TIM3->CCR3 = k; 
         EXTI_clear(EXTI6);
     }
 }
 
 void loop(void)
 {
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