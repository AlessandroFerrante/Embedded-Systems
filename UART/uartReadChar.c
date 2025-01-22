/**
 * @file uartReadchar.c
 * @author Alessandro Ferrante (github@alessandroferrante.net)
 * @brief This file contains the implementation for reading a character from UART.
 * 
 * The code in this file is responsible for interfacing with the UART hardware to read a single character.
 * It includes functions to initialize the UART, configure its settings, and read data from the UART buffer.
 * The functions that concern UART belong to the `stm32_unict_lib.h` library.
 * @version 0.1
 * @date 2024-11-20
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "stm32_unict_lib.h"
#include <stdlib.h>

void setup(void)
{
    ClockConfig();

    CONSOLE_init();
}

void loop(void)
{
    int c = readchar();
    printf("The character type has code = %x (%c) \n", c, c);
}

int main()
{
    setup();
    for (;;) {
        loop();
    }
}