/**
 * @file main.c
 * @author Alessandro Ferrante (github@alessandroferrante.net)
 * @brief 
 * @version 0.1
 * @date 2025-02-20
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <stdio.h>
#include <string.h>
#include "stm32_unict_lib.h"

typedef enum {
    ON,
    OFF,
    FLASHING
} state;

void setup(){

    ClockConfig();
    CONSOLE_init();
	delay_ms(2000);
    printf("Starting\n");
    DISPLAY_init();

    GPIO_init(GPIOB); // initialize port B
    GPIO_init(GPIOC); // initialize port C
    GPIO_config_output(GPIOB, 0); // configure pin PB0 as output
    GPIO_config_output(GPIOC, 2); // configure pin PC2 as output
    GPIO_config_output(GPIOC, 3); // configure pin PC2 as output
    GPIO_config_output(GPIOB, 8);

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
    TIM_config_timebase(TIM2, 8400, 10000); // 1000ms
    TIM_enable_irq(TIM2, IRQ_UPDATE);
    TIM_set(TIM2, 0);
    TIM_on(TIM2);

    TIM_init(TIM3);
    TIM_config_timebase(TIM3, 8400, 5000); //500ms 
    TIM_enable_irq(TIM3, IRQ_UPDATE);
    TIM_set(TIM3, 0);
    TIM_on(TIM3);

    // ADC
    ADC_init(ADC1, ADC_RES_10, ADC_ALIGN_RIGHT); 
    ADC_channel_config(ADC1, GPIOC, 0, 10);
    ADC_on(ADC1);
    ADC_sample_channel(ADC1, 10); 
}

state state_R = OFF;
state state_Y = OFF;
state state_G = OFF;
state state_config = OFF;
state state_config2 = OFF;

int Tr = 10;
int Ty = 10;
int Tg = 10;
float Tr_sup;
float Ty_sup;
float Tg_sup;
int trim_intervall = 0;
int time_ss = 0;
int time_mm = 0;

void TIM2_IRQHandler(void){
    if (TIM_update_check(TIM2)){
        // ? operating time
        time_ss++;
        if(time_ss > 59) {
            time_ss = 0;
            time_mm++;
        }
        if(time_mm > 59) {
            time_mm = 0;
        }
        
        if (state_R == ON){
            GPIO_write(GPIOB, 0, 1);
            Tr_sup--;
            if (Tr_sup == 3){
                state_R = FLASHING;
            }   
        }

        if (state_Y == ON){
            GPIO_write(GPIOC, 2, 1);
            Ty_sup--;
            if (Ty_sup == 3){
                state_Y = FLASHING;
            }   
        }

        if (state_G == ON){
            GPIO_write(GPIOC, 3, 1);
            Tg_sup--;
            if (Tg_sup == 3){
                state_G = FLASHING;
            }   
        }
        TIM_update_clear(TIM2);	
    }
}

void TIM3_IRQHandler(void){

    if (TIM_update_check(TIM3)) {
        // ? for operating time
        if(state_config == OFF){
            GPIO_toggle(GPIOB, 8);
        }

        if (state_R == FLASHING){
            Tr_sup-=0.5;
            GPIO_toggle(GPIOB, 0);
            if (Tr_sup == 0){
                state_R = OFF; 
                GPIO_write(GPIOB, 0, 0);
            }
        }

        if (state_Y == FLASHING){
            Ty_sup-=0.5;
            GPIO_toggle(GPIOC, 2);
            if (Ty_sup == 0){
                state_Y = OFF; 
                GPIO_write(GPIOC, 2, 0);
            }    
        }

        if (state_G == FLASHING){
            Tg_sup-=0.5;
            GPIO_toggle(GPIOC, 3);
            if (Tg_sup == 0){
                state_G = OFF; 
                GPIO_write(GPIOC, 3, 0);
            }  
        }

        TIM_update_clear(TIM3);	
	}
}

void EXTI15_10_IRQHandler(void){
    if (EXTI_isset(EXTI10)) {       
        // button X 
        //GPIO_write(GPIOB, 0, 1);
        Tr_sup = Tr;
        state_R = ON;
        /*if (state_config == ON){
            char s_inteval_red[5];
            Tr = trim_intervall;
            sprintf(s_inteval_red, "r: %d", trim_intervall);
            DISPLAY_puts(0, s_inteval_red);
            delay_ms(4000);
            state_config = OFF;
        }*/
        
        EXTI_clear(EXTI10);
    }
}

void EXTI4_IRQHandler(void){

    if (EXTI_isset(EXTI4)) {
        // button Y 
        //GPIO_write(GPIOC, 2, 1);
        Ty_sup = Ty;
        state_Y = ON;

        EXTI_clear(EXTI4);
    }
}

void EXTI9_5_IRQHandler(void){
    if (EXTI_isset(EXTI5)) {
        // button Z 
        //GPIO_write(GPIOC, 3, 1);
        Tg_sup = Tg;
        state_G = ON;

        EXTI_clear(EXTI5);
    }

    if (EXTI_isset(EXTI6)) {
        // button T   
        if(state_config == OFF){
            GPIO_write(GPIOB, 8, 0);
            state_config = ON;           
            printf("Press X, Y or Z to configure the time of the red, yellow or green LED time with the value of the trimmerr \n");
         } else {
            // Cancel the current configuration process
            state_config = OFF;
            printf("Configuration cancelled\n");
        }
        EXTI_clear(EXTI6);
    }
}


void loop(){
    // ADC for trimmer to change the LED time
    ADC_start(ADC1);
    while (!ADC_completed(ADC1)){ }
    int value = ADC_read(ADC1);
    trim_intervall = (value * 6) / 1023 + 4;
    char s[5];
   
    // ? for operating time
    if(state_config == OFF){
        sprintf(s, "%2d%2d", time_mm, time_ss);
        DISPLAY_puts(0, s);
    }

    if(state_config == ON){
        sprintf(s, "%4d", trim_intervall);
        DISPLAY_puts(0, s);

        char s_inteval[5];
        if (kbhit()){
            int c = readchar();
            switch (c){
            case 'X':
                printf("r: %d\n", trim_intervall);
                sprintf(s_inteval, "r: %d", trim_intervall);
                DISPLAY_puts(0, s_inteval);
                Tr = trim_intervall;
                delay_ms(4000);
                state_config = OFF;
                break;
            case 'Y': 
                printf("y: %d\n", trim_intervall);
                sprintf(s_inteval, "y: %d", trim_intervall);
                DISPLAY_puts(0, s_inteval);
                Ty = trim_intervall;
                delay_ms(4000);
                state_config = OFF;
                break;
            case 'Z':
                printf("g: %d\n", trim_intervall);
                sprintf(s_inteval, "G: %d", trim_intervall);
                DISPLAY_puts(0, s_inteval);
                Tg = trim_intervall;
                delay_ms(4000);
                state_config = OFF;
                break;
            default:
                printf("Invalid option\n");
                break;
            } 
        }
    }
    
    // - *********************
    // - ******* lOG ********
    // - *******************

    static state prev_state_R = OFF;
    static state prev_state_Y = OFF;
    static state prev_state_G = OFF;

    // led red
    if (state_R != prev_state_R) {
        if (state_R == OFF) {
            printf("Led red : OFF\n");
        } else if (state_R == ON) {
            printf("Led red : ON\n");
        } else if (state_R == FLASHING) {
            printf("Led red : FLASHING\n");
        }
        prev_state_R = state_R;
    }
    // led yellow
    if (state_Y != prev_state_Y) {
        if (state_Y == OFF) {
            printf("Led yellow : OFF\n");
        } else if (state_Y == ON) {
            printf("Led yellow : ON\n");
        } else if (state_Y == FLASHING) {
            printf("Led yellow : FLASHING\n");
        }
        prev_state_Y = state_Y;
    }

    // led green
    if (state_G != prev_state_G) {
        if (state_G == OFF) {
            printf("Led green : OFF\n");
        } else if (state_G == ON) {
            printf("Led green : ON\n");
        } else if (state_G == FLASHING) {
            printf("Led green : FLASHING\n");
        }
        prev_state_G = state_G;
    }

}

int main(void){
    setup();
    for (;;){
       loop(); 
    }
}
