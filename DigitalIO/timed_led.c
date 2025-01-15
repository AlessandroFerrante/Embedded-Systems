/**
 * @file timed_led.c
 * @author Alessandro Ferrante (github@alessandroferrante.net)
 * @brief 
 * @version 0.1
 * @date 2024-10-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "stm32_unict_lib.h"

typedef enum {
    LED_OFF,
    LED_ON
} t_led_state;

int prev_state;
t_led_state led_state = LED_OFF;
int delay_counter;

void setup(void)
{
    ClockConfig();
    // configure the clock @ 84 MHz
    GPIO_init(GPIOB); // initialize port B
    GPIO_config_output(GPIOB, 0); // configure pin PB0 as output
    GPIO_config_input(GPIOB, 10); // configure pin PB10 as input
    prev_state = GPIO_read(GPIOB, 10); // read PB10
}

void loop(void)
{
    int current_state = GPIO_read(GPIOB, 10); // read PB10
    if (prev_state == 1 && current_state == 0) {
        // falling edge detected
        switch (led_state) {
            case LED_OFF:
                led_state = LED_ON;
                delay_counter = 0;
                break;
            case LED_ON:
                delay_counter = 0;
                break;
        }
    }
    if (led_state == LED_ON) {
        delay_ms(10);
        delay_counter++;
        if (delay_counter == 1000) {
            led_state = LED_OFF;
        }
    }

    if (led_state == LED_ON)
        GPIO_write(GPIOB, 0, 1);
    else
        GPIO_write(GPIOB, 0, 0);

    prev_state = current_state;

}

int main()
{
    setup();
    for (;;) {
        loop();
    }
}