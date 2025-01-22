# UART Protocol
This directory contains various implementations of UART with the `stm32_unict_lib.h` library for STM32 microcontroller. 
UART (Universal Asynchronous Receiver/Transmitter) is a hardware communication protocol that uses asynchronous serial 
communication with configurable speed. It is commonly used for communication between microcontrollers 
and other devices such as sensors, computers, and other microcontrollers.

UART communication involves two main components: the transmitter and the receiver. 
The transmitter converts parallel data from the microcontroller into serial form and sends it over a communication channel. 
The receiver converts the serial data back into parallel form for the microcontroller to process.

Key features of UART:
- Asynchronous communication: No clock signal is required.
- Configurable baud rate: The speed of communication can be adjusted.
- Full-duplex communication: Data can be transmitted and received simultaneously.
- Error detection: Parity bits can be used to detect errors in the transmitted data.

In this directory, you will find implementations of UART using the `stm32_unict_lib.h` library for the STM32 microcontroller.

## File
- [uart.c](https://github.com/AlessandroFerrante/Embedded-Systems/blob/main/UART/uart.c)
- [uartReadChar.c](https://github.com/AlessandroFerrante/Embedded-Systems/blob/main/UART/uartReadChar.c)
- [uartReadCharNotBlocking.c](https://github.com/AlessandroFerrante/Embedded-Systems/blob/main/UART/uartReadCharNotBlocking.c)

## Requirements
- Visual Studio Code with the [PlatformIO](https://platformio.org/).
- `stm32_unict_lib` library provided by the university for the STM32 microcontroller.

## License
This project is licensed under the MIT license. See the `LICENSE` file for more details.

## Authors
**Alessandro Ferrante**