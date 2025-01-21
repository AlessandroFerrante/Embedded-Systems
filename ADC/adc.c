/**
 * @file adc.c
 * @author Alessandro Ferrante (github@alessandroferrante.net)
 * @brief This file contains the implementation of the Analog-to-Digital Converter (ADC) functionality.
 * 
 * The ADC module is responsible for converting analog signals into digital values that can be processed
 * by the microcontroller. This is essential for interfacing with various analog sensors and devices.
 * 
 * The code in this file initializes the ADC, configures the necessary settings, and provides functions
 * to start conversions and read the digital values from the ADC channels.
 * 
 * Key functionalities include:
 * - ADC initialization and configuration
 * - Starting ADC conversions
 * - Reading ADC conversion results
 * 
 * The ADC is a crucial component in embedded systems for tasks such as sensor data acquisition, 
 * signal processing, and control applications.
 * @version 0.1
 * @date 2024-10-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <stdio.h>
#include "stm32_unict_lib.h"
int new_arr_value = 100;

void main(void)
{
    ClockConfig();

    DISPLAY_init();

    GPIO_init(GPIOB);
    GPIO_config_output(GPIOB, 0);

    ADC_init(ADC1, ADC_RES_8, ADC_ALIGN_RIGHT);

    // initializes and configure the ADC
    ADC_channel_config(ADC1, GPIOC, 0, 10);
    // start ADC comunication
    ADC_on(ADC1);
    // set ADC channel
    ADC_sample_channel(ADC1, 10);

    TIM_init(TIM2);
    TIM_config_timebase(TIM2, 42000, 100);
    TIM_set(TIM2, 0);
    TIM_enable_irq(TIM2, IRQ_UPDATE);
    TIM_on(TIM2);

    for (;;)
    {
        ADC_start(ADC1);
        while (!ADC_completed(ADC1))
        {
        }
        int value = ADC_read(ADC1); // read ADC value

        // to map an interval of values ​​in another interval.
        new_arr_value = (value * 900) / 255 + 100; 
        
        char s[5];
        sprintf(s, "%4d", new_arr_value / 2); // we will display the milliseconds
        DISPLAY_puts(0, s);
    }
}

void TIM2_IRQHandler(void)
{
    if (TIM_update_check(TIM2))
    {
        GPIO_toggle(GPIOB, 0);
        TIM_update_clear(TIM2);
        // update the autoreload register with new value
        TIM2->ARR = new_arr_value;
       
    }
}