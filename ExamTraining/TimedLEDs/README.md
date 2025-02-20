# LED controller with user interface

This project demonstrates the control of LEDs using an STM32 board. The code allows you to manage the switching on, off and flashing of three LEDs (red, yellow, green) using buttons and a trimmer to adjust the times.

## Description

The project uses an STM32 board to control three LEDs (red, yellow and green) connected to pins PB0, PC2 and PC3 respectively. The user can interact with the system using four buttons connected to pins PB10, PB4, PB5 and PB6. A trimmer connected to ADC1 (channel 10) allows you to vary the switching on times of the LEDs.

### Main features:

* **LED control**: Switching on, off and flashing of the red, yellow and green LEDs.
* **Buttons**:
* Button X (PB10): Activates the red LED.
* Button Y (PB4): Activates the yellow LED.
* Z button (PB5): Activates the green LED.
* T button (PB6): Enters/exits configuration mode to set the LED timing via trimmer.
* **Trimmer**: Adjustment of the LED lighting timing in configuration mode.
* **Timer**: Use of the TIM2 and TIM3 timers to manage timing and flashing.
* **ADC**: Analog-to-digital conversion to read the trimmer value.
* **Display**: Display of timing and system status on a display.
* **Serial output**: Log messages on the LED status.

## Operation:
* Press the X, Y, Z buttons to turn on the respective LEDs (red, yellow, green).
* Press the T button to enter configuration mode. In this mode, the trimmer value is displayed on the display.
* Rotate the trimmer to change the value.
* Press X, Y or Z to set the timing of the respective LED with the current trimmer value.
* Press T again to exit configuration mode.


## Requirements
- Visual Studio Code with the [PlatformIO](https://platformio.org/).
- `stm32_unict_lib` library provided by the university for the STM32 microcontroller.

## License
This repository is licensed under the MIT license. See the `LICENSE` file for more details.

## Author
**Alessandro Ferrante**