/**
 * @file main.c
 * @author Alessandro Ferrante (github@alessandroferrante.net)
 * @brief 
 * @version 0.1
 * @date 2025-02-18
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include "stm32_unict_lib.h"


typedef enum {
    C_ON, 
    C_OFF
} chronos_state;

typedef enum {
    M_ON,
    M_OFF,
} memory_state;

typedef enum {
    S_ON,
    S_OFF
} setting_state;

chronos_state c_state = C_OFF;
memory_state m_state = M_OFF;
memory_state flag_count = M_OFF;
setting_state setting_flag = S_OFF; 

typedef struct {
    int ss;
    int cc;
} ss_cc;

/*
typedef struct {
    int size;     // Numero di elementi attualmente presenti
    int queueSize; // Capacità massima della coda
    int head;     // Punto di inserimento
    int tail;     // Punto di rimozione
    ss_cc *queue; // Array dinamico per memorizzare i dati
} chronometer_queue;

chronometer_queue snapshots;

void init_queue(int n) {
    snapshots.size = 0;    // Numero di elementi attualmente presenti
    snapshots.queueSize = 10; // Capacità massima della coda
    snapshots.head = 0;     // Punto di inserimento
    snapshots.tail = 0;     // Punto di rimozione
    snapshots.queue = calloc(n, sizeof(ss_cc)); // Allocazione e azzeramento
}

void enqueue_snapshot(int cc, int ss) {
    snapshots.queue[snapshots.tail].cc = cc;
    snapshots.queue[snapshots.tail].ss = ss;
    snapshots.tail = (snapshots.tail + 1) % snapshots.queueSize;
    if (snapshots.size < snapshots.queueSize)
        snapshots.size++;
    else
        snapshots.head = (snapshots.head + 1) % snapshots.queueSize;
}

void print_queue() {
    for (int i = 0; i < snapshots.size; i++) {
        int index = (snapshots.head + i) % snapshots.queueSize;
        printf("[%d] ss=%d, cc=%d\n", i, snapshots.queue[index].ss, snapshots.queue[index].cc);
    }
}
*/

struct chronometer_snapshots{
    int size; 
    int count;    
    ss_cc *times; 
} snapshots;

void init_array(int k) {
    snapshots.size = k;
    snapshots.count = 0;
    flag_count = M_OFF;
    snapshots.times = malloc(k * sizeof(ss_cc));
    for (size_t i = 0; i < k; i++) {
        snapshots.times[i].ss = 0;
        snapshots.times[i].cc = 0;
    }
}

void insert_snapshot(int cc, int ss) {
    if (snapshots.count >= snapshots.size){
        snapshots.count = 0;
        flag_count = M_ON;
    }
    if (snapshots.count < snapshots.size) {
        snapshots.times[snapshots.count].cc = cc;
        snapshots.times[snapshots.count].ss = ss;
        snapshots.count++;
    }
} 

void print_queue() {
    if(flag_count == M_OFF)
        for (int i = 0; i < snapshots.count; i++) {
            printf("[%d] ss=%d, cc=%d\n", i, snapshots.times[i].ss, snapshots.times[i].cc);
        }
    
    if(flag_count == M_ON)  
        for (int i = 0; i < snapshots.size; i++) {
            printf("[%d] ss=%d, cc=%d\n", i, snapshots.times[i].ss, snapshots.times[i].cc);
        }
}

int cc = 0;
int ss = 0;
int k = 10;

void TIM2_IRQHandler(void){

    if (TIM_update_check(TIM2)){
        if (c_state == C_ON && setting_flag == S_OFF){
            char s_cc[5];
            sprintf(s_cc,"%4d",cc);
            DISPLAY_puts(0, s_cc);
            cc++;
            if (cc > 99) {
                cc = 0;
                ss++;
            }         
            if(ss>59){
                ss = 0;
            }     
            char s_ss[5];
            sprintf(s_ss,"%2d", ss);
            DISPLAY_puts(0, s_ss);
        }
        
        TIM_update_clear(TIM2);	
    }

}

int second = 0;
void TIM3_IRQHandler(void){

    if (TIM_update_check(TIM3)) {
        
        GPIO_toggle(GPIOB, 8);      
        
        if(m_state == M_ON){
            second++;
            if (second == 2){
                GPIO_toggle(GPIOB, 0);
                second = 0;
                m_state = M_OFF;
            }
        }     
        
        TIM_update_clear(TIM3);	
	}
}


void EXTI15_10_IRQHandler(void){

    if (EXTI_isset(EXTI10)) {       
        // button X 
        if(setting_flag == S_OFF)
            c_state = !c_state; // state is C_OFF or C_ON that are 0 or 1
        
        if(setting_flag == S_ON && c_state == C_OFF){
            init_array(k);
            setting_flag = S_OFF;
            c_state = C_OFF;
            printf("Press 'S' to show the content of the memory \n"
                    "Press 'C' to configure the size of the memory \n"
                    "Press 'E' to return to normal functions \n");
            
            int c = readchar();    
            switch (c){
                case 'S':
                    print_queue();
                    c_state = C_ON;
                    setting_flag = S_OFF; 
                    break;
                case 'C':
                    setting_flag = S_ON;  
                    printf("Imposta il valore di K con la manopola e conferma con il pulsante X\n");
                    
                    break;
                case 'E':
                    printf("Returning to normal functions\n");
                    c_state = C_ON;
                    setting_flag = S_OFF; 
                    EXTI_clear(EXTI10);
                    break;
                default:
                    printf("Invalid option\n");
                    c_state = C_OFF;
                    printf("Press 'S' to show the content of the memory \n"
                            "Press 'C' to configure the size of the memory \n"
                            "Press 'E' to return to normal functions \n");

                    int c = readchar(); 
                    break;
            }
        }
        EXTI_clear(EXTI10);
    }
}

void EXTI4_IRQHandler(void){

    if (EXTI_isset(EXTI4)) {
        // button Y 
        // clear the chronometer
        cc = 0;
        ss = 0;
        char s_cc[5];
        sprintf(s_cc,"%4d",cc);
        DISPLAY_puts(0, s_cc);
        char s_ss[5];
        sprintf(s_ss,"%2d", ss);
        DISPLAY_puts(0, s_ss);
        //c_state = C_OFF;
        EXTI_clear(EXTI4);
    }
}

void EXTI9_5_IRQHandler(void){

    if (EXTI_isset(EXTI5)) {
        // button Z
        insert_snapshot(cc, ss);
        GPIO_write(GPIOB, 0, 1);
        m_state = M_ON;
        if(c_state == C_OFF)
            init_array(15);
        
        print_queue();
        EXTI_clear(EXTI5);
    }

    if (EXTI_isset(EXTI6)) {
        // button T   
        c_state = C_OFF;
        printf("Press 'S' to show the content of the memory \n"
               "Press 'C' to configure the size of the memory \n"
               "Press 'E' to return to normal functions \n");
        
        int c = readchar();    
        switch (c){
            case 'S':
                print_queue();
                c_state = C_ON;
                setting_flag = S_OFF; 
                break;
            case 'C':
                setting_flag = S_ON;  
                printf("Imposta il valore di K con la manopola e conferma con il pulsante X. \n");
                
                break;
            case 'E':
                printf("Returning to normal functions\n");
                c_state = C_ON;
                setting_flag = S_OFF; 
                EXTI_clear(EXTI6);
                break;
            default:
                printf("Invalid option\n");
                printf("Returning to normal functions\n");
                c_state = C_ON;
                setting_flag = S_OFF; 
                break;
        }
        
        EXTI_clear(EXTI6);
    }
} 

void setup(void){
    ClockConfig();

    DISPLAY_init();
    
    CONSOLE_init();
	delay_ms(2000);
    printf("Starting\n");

    
    GPIO_init(GPIOB);
    GPIO_init(GPIOC);

    GPIO_config_input(GPIOB, 4);
    GPIO_config_input(GPIOB, 5);
    GPIO_config_input(GPIOB, 6);
    GPIO_config_input(GPIOB, 10);

    GPIO_config_output(GPIOB, 0);
    GPIO_config_output(GPIOB, 8);
    GPIO_config_output(GPIOC, 2);
    GPIO_config_output(GPIOC, 3);
    
    GPIO_config_EXTI(GPIOB, EXTI10); // attach PB10 to EXTI10
    GPIO_config_EXTI(GPIOB, EXTI4); // attach PB4 to EXTI4
    GPIO_config_EXTI(GPIOB, EXTI5); // attach PB5 to EXTI5
    GPIO_config_EXTI(GPIOB, EXTI6); // attach PB6 to EXTI6

    EXTI_enable(EXTI10, FALLING_EDGE);
    EXTI_enable(EXTI4, FALLING_EDGE);
    EXTI_enable(EXTI5, FALLING_EDGE);
    EXTI_enable(EXTI6, FALLING_EDGE);

    TIM_init(TIM2);
    TIM_config_timebase(TIM2, 8400, 100); //10ms, 1cs 
    // (8400prescaler * 100autoreload) / 84000clk = 10 ms
    TIM_enable_irq(TIM2, IRQ_UPDATE);
    TIM_set(TIM2, 0);
    TIM_on(TIM2);

    TIM_init(TIM3);
    TIM_config_timebase(TIM3, 8400, 5000); //500ms 
    // (8400prescaler * 5000autoreload) / 84000clk = 500 ms
    TIM_enable_irq(TIM3, IRQ_UPDATE);
    TIM_set(TIM3, 0);
    TIM_on(TIM3);

    init_array(k); 
    
    ADC_init(ADC1, ADC_RES_10, ADC_ALIGN_RIGHT); // ADC1, 8bit, 
    ADC_channel_config(ADC1, GPIOC, 0, 10);
    ADC_on(ADC1);
    ADC_sample_channel(ADC1, 10);
}

void loop(void){    
    if(setting_flag == S_ON){
        ADC_sample_channel(ADC1, 10);
        ADC_start(ADC1);
        while (!ADC_completed(ADC1)){ }
        int value = ADC_read(ADC1);
        k = (value * 10) / 1023 + 5;
        //printf("%d\n", k);
        char s[5];
        sprintf(s, "%4d", k);
        DISPLAY_puts(0, s);
    }
}

int main(){
    setup();
    for (;;)
    {
        loop();
    }
}