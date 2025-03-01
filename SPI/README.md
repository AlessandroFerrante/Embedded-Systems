# SPI (Serial Peripheral Interface)
SPI is a synchronous serial communication protocol used to connect microcontrollers (MCUs) to peripherals.

## Description

SPI is a versatile and widely used communication interface for communicating between integrated circuits over short distances. It is particularly suitable for communication between a microcontroller and peripherals such as sensors, memories, displays and other devices. SPI is a **Master-Slave** interface, where the Master controls the communication and the Slaves respond to its requests.

**Key features of the SPI interface:**

* **Synchronous serial communication**: Data is transferred bit by bit synchronously with a clock signal.
* **Master-Slave**: One device (the Master) controls the communication and one or more devices (the Slaves) respond to its requests.
* **Full-duplex**: Communication can occur in both directions simultaneously.
* **Low Complexity**: SPI is relatively simple to implement in both hardware and software.

### SPI Signals

The SPI interface uses four main signals:

* **SCK (Serial Clock)**: Clock signal generated by the Master to synchronize communication.
* **MOSI (Master Out Slave In)**: Data line for data transmission from the Master to the Slave.
* **MISO (Master In Slave Out)**: Data line for data transmission from the Slave to the Master.
* **SS (Slave Select)**: Signal used by the Master to select a specific Slave for communication. It is sometimes referred to as CS (Chip Select).

### Data Transmission

SPI communication is packet-based, where a packet is a set of bits whose meaning is defined by the slave device to which it is addressed. The Master always starts an SPI transaction by setting the SS line to logic "0". The Master then generates the clock signal on the SCK line. At each clock pulse:

* The Master sends a data bit on the MOSI line.
* The Slave sends a data bit on the MISO line.

When the transaction completes, the Master sets the SS line to logic "1".

### Clock Polarity and Phase

The clock polarity (CPOL) and clock phase (CPHA) can be configured to match the specifications of the slave device.

* **CPOL (Clock Polarity)**: Specifies the initial logical state of the clock.
* **CPHA (Clock Phase)**: Specifies the clock transition used to sample the data.

### Connecting Multiple Slaves

There are two main ways to connect multiple Slave devices to a single SPI Master:

* **Parallel Connection**: SCK, MOSI and MISO are connected together, and each slave has its own SS line.
* **Daisy-Chain**: MOSI and MISO are daisy-chained between slave devices, and there is a single SS line for all devices.

# MAX7219 Sensor

This repository contains code to control a **MAX7219** based LED matrix display using an **STM32** board and **SPI (Serial Peripheral Interface)** communication.

## Description

The code provides functions to **initialize, configure, and control** a MAX7219 based LED matrix display using an STM32 board and SPI communication. It allows you to send data and commands to the display to control the LEDs by manipulating the MAX7219 registers.

## Files
- [max7219Driver.c](https://github.com/AlessandroFerrante/Embedded-Systems/blob/main/I2C/ColorSensor/tcs.c): this file does not use SPI and manages the sensor through GPIO and functions;
- [max7219SPIdriver.c](https://github.com/AlessandroFerrante/Embedded-Systems/blob/main/I2C/MEMS/mems.c): this file uses **SPI communication** to interact with the MAX7219. The Master (STM32) generates the clock (SCK) and sends data (MOSI) to the Slave (MAX7219). The SS (Slave Select) signal is used to activate the MAX7219 chip. This code **directly manipulates the MAX7219 registers** to configure the display and control its LEDs.

## Requirements
- Visual Studio Code with the [PlatformIO](https://platformio.org/).
- `stm32_unict_lib` library provided by the university for the STM32 microcontroller.

## License
This project is licensed under the MIT license. See the `LICENSE` file for more details.

## Authors
**Alessandro Ferrante**