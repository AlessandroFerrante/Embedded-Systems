/**
 * @file nb_readchar.c
 * @author Alessandro Ferrante (github@alessandroferrante.net)
 * @brief This file contains the implementation for non-blocking character reading from UART.
 * 
 * The functions in this file read characters from a UART interface without blocking the execution of the program.
 * This is useful in embedded systems where efficient and responsive communication is required.
 * The functions that concern UART belong to the `stm32_unict_lib.h` library.
 * @version 0.1
 * @date 2024-11-20
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "stm32_unict_lib.h"
#include <stdio.h>

void setup(void)
{
    ClockConfig();
    CONSOLE_init();
}

int i = 0;
void loop(void)
{
    printf("uart, %d\n", i);
    i++;
    if (kbhit()) {
        readchar();
        printf("STOP!\n");
        readchar();
    }
}

int main()
{
    setup();
    for (;;) {
        loop();
    }
}