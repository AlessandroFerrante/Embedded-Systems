/**
 * @file automaticGate.c
 * 
 * @author Alessandro Ferrante (github@alessandroferrante.net)
 * @brief This file contains the main implementation for the Automated Gate system.
 * 
 * This module is responsible for initializing the system, handling the main control loop,
 * and managing the interactions between different components of the Automated Gate system.
 * 
 * The system operates in different states: IDLE, OPENING, WAIT, CLOSING, and STOPPED.
 * - IDLE: The gate is closed and waiting for an open command.
 * - OPENING: The gate is in the process of opening.
 * - WAIT: The gate is fully open and waiting for a close command after a delay.
 * - CLOSING: The gate is in the process of closing.
 * - STOPPED: The gate operation is halted.
 * 
 * The system uses timers (TIM2 and TIM3) to manage the timing of gate operations and external interrupts (EXTI)
 * to handle button presses for controlling the gate state transitions.
 * 
 * The display shows the gate's opening percentage using a simple visual representation.
 * 
 * This implementation is designed to run on an STM32 microcontroller.
 * @version 0.1
 * @date 2024-10-
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "stm32_unict_lib.h"
#include <string.h>
#include <stdbool.h>

typedef enum {
    IDLE,
    OPENING,
    WAIT,
    CLOSING,
    STOPPED
}t_state;


t_state state = IDLE;
int percentage = 0;
int t_wait = 0;

void setup(void)
{
    ClockConfig();
    // configure the clock @ 84 MHz
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
	TIM_config_timebase(TIM2, 8400, 5000);//500ms
	TIM_enable_irq(TIM2, IRQ_UPDATE);
    TIM_set(TIM2, 0);
	TIM_on(TIM2);

    TIM_init(TIM3);
	TIM_config_timebase(TIM3, 8400, 1000);//100ms
	TIM_enable_irq(TIM3, IRQ_UPDATE);
    TIM_set(TIM3, 0);
	TIM_on(TIM3);
    
}

void TIM2_IRQHandler(void){
	if (TIM_update_check(TIM2) && (state==OPENING || state== CLOSING)) {
        GPIO_write(GPIOC, 2, 0);
        GPIO_write(GPIOC, 3, 0);
        GPIO_toggle(GPIOB, 0);
        percentage++;
        TIM_update_clear(TIM2);	
	}
    else if(state == WAIT)        
        GPIO_write(GPIOC, 2, 1);
    else 
        GPIO_write(GPIOC, 3, 1);
}

void TIM3_IRQHandler(void){
    if (TIM_update_check(TIM3)) {
        
        switch(state){
    
        case OPENING:
            percentage++;
            if (percentage >= 100)
                state = WAIT;
            break;

        case CLOSING: 
            percentage--;
            if(percentage == 0)
                state = IDLE;
            break;
            
        case WAIT: 
            t_wait++;
            if (t_wait == 40){
                t_wait = 0;
                state = CLOSING;
            }
            break;
        case STOPPED:
            GPIO_write(GPIOB, 0, 1);
            GPIO_write(GPIOC, 2, 1);    
            GPIO_write(GPIOC, 3, 1);
            break;
        default:
            GPIO_write(GPIOB, 0, 0);
            break;
        
        }
        TIM_update_clear(TIM3);	
	}
}

void EXTI15_10_IRQHandler(void)
{
    if (EXTI_isset(EXTI10)) {       
        // button X 
        if(state == IDLE || state == CLOSING || state == STOPPED)
            state = OPENING;
        EXTI_clear(EXTI10);
    }
}

 

void EXTI4_IRQHandler(void)
{
    if (EXTI_isset(EXTI4)) {
        // button Y 
        if(state == WAIT || state == OPENING || state == STOPPED)
            state = CLOSING;
        EXTI_clear(EXTI4);
    }
}

void EXTI9_5_IRQHandler(void)
{
    if (EXTI_isset(EXTI5)) {
        // button Z 
        switch(state){
        case OPENING:
            break;
        case WAIT:
            t_wait = 0;
            break;
        case CLOSING:
            state = OPENING;
            break;
        }
        
        EXTI_clear(EXTI5);
    }
    if (EXTI_isset(EXTI6)) {
        // button T      
        if(state == OPENING || state == CLOSING || state == WAIT)
            state = STOPPED;
        EXTI_clear(EXTI6);
    }
}


void loop(void)
{
    char s[5];
    if(percentage < 25)
        strcpy(s, "----"); 
    else if(percentage < 50)
        strcpy(s, "--- "); 
    else if(percentage < 75)
        strcpy(s, "--  "); 
    else if(percentage < 100)
        strcpy(s, "-   "); 
    else
        strcpy(s, "    ");
    //sprintf(s, "%4d", percentage);
    DISPLAY_puts(0, s);
}

int main()
{
    setup();
    for (;;) {
        loop();
    }
}