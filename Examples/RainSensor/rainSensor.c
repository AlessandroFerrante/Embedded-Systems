/**
 * @file RainsSensor.c
 * @author Alessandro Ferrante (github@alessandroferrante.net)
 * @brief This file contains the implementation of a rain sensor to manage the windshield wipers. 
 *
 * The trimmer simulates the variation of the intensity of the rain.
 * The rain sensor is then responsible for reading the analog data
 * and converting it into a digital value using the ADC (Analog-to-Digital Converter).
 * The converted digital value is used to determine the presence and intensity of rain.
 * The intensity of the rain determines the speed of the windshield wipers 
 *
 * @version 0.1
 * @date 2024-10-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <stdio.h>
#include <string.h>
#include "stm32_unict_lib.h"

typedef enum {
    OFF,
    TO_OFF,
    RAIN,
    TIMED_ON,
    TIMED_OFF,
    CONTINUE
} states;

#define TS1 200
#define TS2 50
char * wipers_display[] = {
    "-   ",
    " -  ",
    "  - ",
    "   -"
    "  - ",
    " -  ",
    "-   "};

states state = OFF;
int wipers_pos = 0;
int wipers_duration, wipers_step_duration; // redundant but functional
int wipers_step_counter = 0;
int wait_time = 0;
int wait_time_counter = 0;
int rain_speed = 0;
int drop_time = 0, drop_time_counter = 0, drops = 0;

void setup(){

    ClockConfig();
    DISPLAY_init();

    GPIO_init(GPIOB); // initialize port B
    GPIO_init(GPIOC); // initialize port C
    GPIO_config_output(GPIOB, 0); // configure pin PB0 as output
    GPIO_config_output(GPIOC, 2); // configure pin PC2 as output
    GPIO_config_output(GPIOC, 3); // configure pin PC2 as output

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
    EXTI_enable(EXTI5, BOTH_EDGES);
    EXTI_enable(EXTI6, FALLING_EDGE);


	TIM_init(TIM2);
	TIM_config_timebase(TIM2, 8400, 10000);//10ms
	TIM_enable_irq(TIM2, IRQ_UPDATE);
    TIM_set(TIM2, 0);
	TIM_on(TIM2);

    TIM_init(TIM3);
	TIM_config_timebase(TIM3, 8400, 1000);//100ms
	TIM_enable_irq(TIM3, IRQ_UPDATE);
    TIM_set(TIM3, 0);
	TIM_on(TIM3);

    // ADC

    ADC_init(ADC1, ADC_RES_10, ADC_ALIGN_RIGHT); // ADC1, 8bit, 
    ADC_channel_config(ADC1, GPIOC, 0, 10);
    ADC_on(ADC1);
    ADC_sample_channel(ADC1, 10);
    /*
    ADC_init(ADC2, ADC_RES_8, ADC_ALIGN_RIGHT); // ADC2, 8bit, 
    ADC_channel_config(ADC2, GPIOC, 0, 10);
    ADC_on(ADC2);
    ADC_sample_channel(ADC2, 10);
    */
}

void TIM2_IRQHandler(void){
    if (TIM_update_check(TIM2)){
        if(state != OFF){

            if (state == RAIN){
                if (rain_speed > 0){
                    ++drop_time_counter;
                    if(drop_time_counter >= drop_time){
                        drop_time_counter = 0;
                        GPIO_toggle(GPIOB, 9);
                        ++drops;
                    }
                }
            }

            int do_wipe = 0;
            switch (state){
            case TO_OFF:
            case TIMED_ON:
            case CONTINUE:
                do_wipe = 1;
                break;
            case RAIN:
                do_wipe = 1;
                drops = 0;
            default:

                break;
            }

            //if (state == CONTINUE || state == TIMED_ON || state == RAIN ){
              if(do_wipe){
                ++wipers_step_counter;
                if(wipers_step_counter >= wipers_step_duration){
                    wipers_step_counter = 0;
                    ++wipers_pos;
                    if(wipers_pos == 8){
                        wipers_pos = 0;
                        switch (state){
                        case TO_OFF:
                            state = OFF;
                        case TIMED_ON:
                            state = TIMED_OFF;
                            break;
                        default:
                            break;
                        }  
                    }            
                }
            }
            else if (state == CONTINUE || state == TIMED_OFF){
                ++wait_time_counter;
                if(wait_time_counter >= wait_time){
                    wait_time_counter = 0;
                    state = TIMED_ON;             
                }
            }
        }
    }
}

void TIM3_IRQHandler(void){
    if (TIM_update_check(TIM3)) {
        
        TIM_update_clear(TIM3);	
	}
}

void EXTI15_10_IRQHandler(void){
    if (EXTI_isset(EXTI10)) {       
        // button X 
        if (state != RAIN){
            state = RAIN;
            
        }
        else    
            state = OFF;
        EXTI_clear(EXTI10);
    }
}

 

void EXTI4_IRQHandler(void)
{
    if (EXTI_isset(EXTI4)) {
        // button Y 
        if (state != TIMED_ON){
            wipers_duration = TS1;
            wipers_step_duration = wipers_duration / 8; 
        }
        else    
            state = OFF;
        EXTI_clear(EXTI4);
    }
}

void EXTI9_5_IRQHandler(void){
    if (EXTI_isset(EXTI5)) {
        // button Z 
        if (state != CONTINUE){
            wipers_duration = TS2;
            wipers_step_duration = wipers_duration / 8; 
        }
        else{
            state = OFF;
            GPIO_write(GPIOC, 2, 0);
        }
        EXTI_clear(EXTI5);
    }
    if (EXTI_isset(EXTI6)) {
        // button T      
        
        EXTI_clear(EXTI6);
    }
}


void loop(){
    if(state != OFF)
        DISPLAY_puts(0, wipers_display[0]);
    else 
        DISPLAY_puts(0, wipers_display[wipers_pos]);

    switch (state){
    case OFF:
        GPIO_write(GPIOB, 0, 0);
        GPIO_write(GPIOC, 2, 0);
        GPIO_write(GPIOC, 3, 0);
        break;
    case RAIN:
        GPIO_write(GPIOB, 0, 1);
        GPIO_write(GPIOC, 2, 0);
        GPIO_write(GPIOC, 3, 0);
        break;
    case TIMED_ON:
        GPIO_write(GPIOB, 0, 0);
        GPIO_write(GPIOC, 2, 1);
        GPIO_write(GPIOC, 3, 0);
        break;
    case CONTINUE:
        GPIO_write(GPIOB, 0, 0);
        GPIO_write(GPIOC, 2, 0);
        GPIO_write(GPIOC, 3, 1);
        break;
    default:
        break;
    }

    if(state == TIMED_ON || state == CONTINUE){
        ADC_sample_channel(ADC1, 10);
        ADC_start(ADC1);
        while (!ADC_completed(ADC1)){ }
        int value = ADC_read(ADC1);
        wait_time = (value * 400) / 1023 + 100;
    }

    if (state == RAIN){
        ADC_sample_channel(ADC1, 11);
        ADC_start(ADC1);
        while (!ADC_completed(ADC1)){ }
        int value = ADC_read(ADC1);
        rain_speed = (value * 500) / 1023 + 100;
        drop_time = 1/ rain_speed;
    }
    
}

void main(void){
    setup();
    for (;;){
       loop(); 
    }
}
