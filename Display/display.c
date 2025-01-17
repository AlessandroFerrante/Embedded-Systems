/**
 * @file main.c
 * @author Alessandro Ferrante (github@alessandroferrante.net)
 * @brief 
 * @version 0.1
 * @date 2024-10-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <string.h>
#include <stdio.h>
#include "stm32_unict_lib.h"

void setup(void)
{
    ClockConfig();
    // configure the clock @ 84 MHz
    DISPLAY_init();
}

int i = 0;

void loop(void)
{
    char s[5];
    sprintf(s,"%4d",i);
    DISPLAY_puts(0, s);
    delay_ms(200);
    i++;
}

int main()
{
    setup();
    for (;;) {
        loop();
    }
}