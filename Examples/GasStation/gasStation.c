/**
 * @file gasStation.c
 * @author Alessandro Ferrante (github@alessandroferrante.net)
 * @brief 
 * The code  implements the control of a gas station using an STM32 microcontroller, 
 * manages the fuel dispensing and the available amount with the `loop`, 
 * uses software delays (`delay_ms`) and the button interrupt to initialize the amount, 
 * during the dispensing, updates the display with the amount dispensed and manages the status LEDs.
 * @version 0.1
 * @date 2024-10-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "stm32_unict_lib.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

float balance = 0;
int delay = 0;
float bGass = 0;
bool deliveryState = false; 


void setup(void) {

    ClockConfig();
    // configure the clock @ 84 MHz
    DISPLAY_init();

    GPIO_init(GPIOB); // initialize port B
    GPIO_init(GPIOC); // initialize port C

    // porte per i led
    GPIO_config_output(GPIOB, 0); // configure pin PB0 as output
    GPIO_config_output(GPIOC, 3); // configure pin PB3 as output
    
    // porte per i pulsanti
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

    
}

void EXTI15_10_IRQHandler(void)
{
    if (EXTI_isset(EXTI10)) {
        // pushbutton X
        balance += 20;
        char s[5];
        sprintf(s,"%4d", (int) balance);
        DISPLAY_puts(0, s);
        EXTI_clear(EXTI10);
    }
}

void EXTI4_IRQHandler(void)
{
    if (EXTI_isset(EXTI4)) {
        // pushbutton Y
        balance += 5;
        char s[5];
        sprintf(s,"%4d", (int) balance);
        DISPLAY_puts(0, s);
        EXTI_clear(EXTI4);
    }
}

void EXTI9_5_IRQHandler(void)
{
    if (EXTI_isset(EXTI5)) {
        // pushbutton Z
        balance = 0;
        char s[5];
        sprintf(s,"%4d", (int) balance);
        DISPLAY_puts(0, s);
        EXTI_clear(EXTI5);
    }
    if (EXTI_isset(EXTI6)) {
        // pushbutton T
        bGass = 0;
        deliveryState = true;
        EXTI_clear(EXTI6);
    }
}


void loop(void) 
{   
    // display for delivery
    if(deliveryState){
        if(bGass > balance-0.75) bGass += balance-bGass;
        if(balance == bGass) {
            balance = 0;
            deliveryState = false;
        }
        else{ bGass += 0.75;}

        delay_ms(500);
        GPIO_toggle(GPIOC, 3);
        delay_ms(500);
     
        
        char s[5];
        sprintf(s,"%4d", (int)bGass);
        DISPLAY_puts(0, s);
        if(bGass >= balance) GPIO_toggle(GPIOB, 0);
    } 
}

int main() 
{
    setup();
    char s[5];
    sprintf(s,"%4d", (int)balance);
    DISPLAY_puts(0, s);
    for (;;) {
        loop();
    }
}