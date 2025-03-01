# Example: Gas Station
These examples implement control of a gas station in two different ways, one with software delay and one with hardware timer.
## Description
- Il file [gasStation.c](https://github.com/AlessandroFerrante/Embedded-Systems/blob/main/Examples/GasStation/gasStation.c) implements the control of a gas station using an STM32 microcontroller, manages the fuel dispensing and the available amount with the `loop`, uses software delays (`delay_ms`) and the button interrupt to initialize the amount, during the dispensing, updates the display with the amount dispensed and manages the status LEDs.
-  Il file [gasStationWithTimers.c](https://github.com/AlessandroFerrante/Embedded-Systems/blob/main/Examples/GasStation/gasStationWithTimers.c) implements the control of a service station using an STM32 microcontroller, manages the fuel dispensing and the user interface through the management functions of the interrupts generated by the timer (TIM3) and by the buttons that control the dispensing and the amount, during the dispensing, updates the display with the amount dispensed and manages the status LEDs.

## Requirements
- Visual Studio Code with the [PlatformIO](https://platformio.org/).
- stm32_unict_lib library provided by the university for the STM32 microcontroller.

## License
This repository is licensed under the MIT license. See the `LICENSE` file for more details.

## Authors
**Alessandro Ferrante**
