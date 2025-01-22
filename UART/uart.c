/**
 * @file uart.c
 * @author Alessandro Ferrante (github@alessandroferrante.net)
 * @brief This file contains the application UART communication.
 *
 * UART (Universal Asynchronous Receiver/Transmitter) 
 * is a hardware communication protocol that uses asynchronous serial 
 * communication with configurable speed. It is commonly used for communication between 
 * microcontrollers and other devices such as sensors, computers, and other microcontrollers.
 *
 * This file sets up and manages UART initialization, with function `CONSOLE_init()` of the `stm32_unict_lib` library.
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

int i = 0;

void loop(void)
{
    printf("UART test, %d\n", i);
    delay_ms(1000);
    i++;
}

int main()
{
    setup();
    for (;;) {
        loop();
    }
}