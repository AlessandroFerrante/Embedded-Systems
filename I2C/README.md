# I2C Communication Protocol

This repository provides an overview of the I2C (Inter-Integrated Circuit) communication protocol.

## What is I2C?

I2C (Inter-Integrated Circuit) is a **serial digital communication bus** standard designed to interconnect integrated circuits (ICs) on the same board. It was first introduced by Philips in the 1980s to interconnect integrated circuits in televisions. It is widely used in various integrated devices that require data communication.

## Key Features

* **Two-wire bus**: SDA (Serial Data) and SCL (Serial Clock).
* **Synchronous communication**: The clock (SCL) is provided by the master to synchronize data transfer.
* **Multiple device support**: A master can communicate with multiple slaves, each identified by a unique address.
* **Speed**: Can reach a maximum speed of 400 Kbps.

## Devices
* **Master**: The device that initiates and controls communication on the I2C bus. Typically a microcontroller (MCU). There can only be one master in an I2C network.
* **Slaves**: Devices that respond to requests from the master. Each slave has a unique address.

## Connections

* **SDA (Serial Data)**: Bidirectional data line used for serial bit transfer.
* **SCL (Serial Clock)**: Unidirectional clock line provided by the master to synchronize transmission.

## Addressing

* **7-bit addresses**: Widely used standard.
* **10-bit addresses**: Used in some special cases.
* **Direction Bit**: An additional bit (B0) specifies the direction of data transfer: "0" for writing and "1" for reading.

* Example: Accelerometer with address `0xd6` :
* Write: `0xd6` (`1 1 0 1 0 1 1 0`)
* Read: `0xd7` (`1 1 0 1 0 1 1 1`)

## I2C Transaction Model

### Write

1. The master sends a **START CONDITION (S)**.
2. The master sends the **device address** with bit B0 set to "0" (write).
3. The master sends the **register number** to write.
4. The master sends the **register data** to write.
5. The previous steps are repeated for each subsequent register to write.
6. The master sends a **STOP CONDITION (P)**.
7. The slave responds with an **ACKNOWLEDGE (ACK)**, a 0 bit sent on the SDA line for each byte written.

### Read

1. The master sends a **START CONDITION (S)**.
2. The master sends the **device address** with bit B0 set to "0" (write).
3. The master sends the **register number** to be read.
4. The master sends a new **START CONDITION (S)**.
5. The master sends the **device address** with bit B0 set to "1" (read).
6. The slave sends the **register data** read.
7. The previous steps are repeated for each subsequent register to be read.
8. The master sends a **STOP CONDITION (P)**.
9. The peer device (master or slave) responds with an **ACKNOWLEDGE (ACK)**, a 0 bit sent on the SDA line for each byte transmitted.

## Timing and Bus States

* **BUS IDLE**: Both the SDA and SCL lines are in the "1" state.
* **START CONDITION (S)**: A high-to-low transition on SDA while SCL is high.
* **STOP CONDITION (P)**: A low-to-high transition on SDA while SCL is high.

## Data Transfer

* Data transfer occurs serially, **MSB-first**.
1. The bit value is set on the SDA line.
2. A low-to-high-to-low pulse occurs on the SCL line.
3. The next bit is sent.
* **ACKNOWLEDGE (ACK)**: After all 8 bits have been transmitted, an acknowledge is expected.
1. The master generates a ninth clock pulse.
2. The receiving device holds the SDA line low to signal that it has understood the sent byte.

## Sending Data to a Slave

1. The master starts communication with a **Start Condition**.
2. The master sends the **Write Command**, an 8-bit data composed of:
* The 7-bit address of the slave device.
* The R/W bit set to 0, meaning "write-to-slave".
3. The addressed slave responds with an ACK, holding the SDA line low in the ninth clock pulse.
4. The master sends an 8-bit data that means the register number.
5. The addressed slave responds with an ACK, holding the SDA line low in the ninth clock pulse.
6. The master sends an 8-bit data that means the register value.
7. The addressed slave responds with an ACK, holding the SDA line low in the ninth clock pulse.
8. The master ends the transmission by sending a **Stop Condition**.

## Receiving Data from a Slave

1. The master starts the communication with a **Start Condition**.
2. The master sends the **Write Command**, an 8-bit data composed of:
* The 7-bit address of the slave device.
* The R/W bit at 0, which means "write-to-slave".
3. The addressed slave responds with an ACK, holding the SDA line low in the ninth clock pulse.
4. After the address, the master sends an 8-bit data value that means register number.
5. The addressed slave responds with an ACK, holding the SDA line low in the ninth clock pulse.
6. The master sends a new **Start Condition**.
7. The master sends the **Read Command**, an 8-bit data value composed of:
* The 7-bit address of the slave device.
* The R/W bit set to 1, meaning "read-from-slave".
8. The addressed slave responds with an ACK, holding the SDA line low in the ninth clock pulse.
9. The slave is now ready to send bytes.
10. The slave sends an 8-bit data value.
11. The master responds with an ACK, holding the SDA line low in the ninth clock pulse.
12. The slave sends the next 8-bit data value (the next register value).
13. The master responds with an ACK, holding the SDA line low on the 9th clock pulse.
14. When the master is no longer interested in the data, it closes the communication by sending a **NACK** (holding the SDA line high on the 9th clock pulse) and then a **Stop Condition**.

## I2C Functions (STM32 unict lib)

* **Initialization**: `void I2C_init(I2C_TypeDef * port, int speed)`
* `port`: Interface (usually `I2C1`).
* `speed`: Clock speed (e.g. `100000`, `400000`).
* **Writing a single register**: `void I2C_write_register(I2C_TypeDef * port, short addr, short reg_adr, short reg_val)`
* `port`: Interface (usually `I2C1`).
* `addr`: Device address.
* `reg_adr`: Register address.
* `reg_val`: Register value.
* **Reading a single register**: `void I2C_read_register(I2C_TypeDef * port, short addr, short reg_adr, short * reg_val)`
* `port`: Interface (usually `I2C1`).
* `addr`: Device address.
* `reg_adr`: Register address.
* `reg_val`: Pointer to the variable that will receive the register value.
* **Writing a set of registers**: `void I2C_write_buffer(I2C_TypeDef * port, short addr, short reg_adr, unsigned char * data, int len)`
* `port`: Interface (usually `I2C1`).
* `addr`: Device address.
* `reg_adr`: Register address.
* `data`: Buffer containing register values.
* `len`: Buffer length (number of registers).
* **Reading a set of registers**: `void I2C_read_buffer(I2C_TypeDef * port, short addr, short reg_adr, unsigned char * data, int len)`
* `port`: Interface (usually `I2C1`).
* `addr`: Device address.
* `reg_adr`: Register address.
* `data`: Buffer that will receive the register values.
* `len`: Length of the buffer (number of registers).

>In this directory, you will find implementations of I2C using the `stm32_unict_lib.h` library for the STM32 microcontroller.

## File
- [mems.c](https://github.com/AlessandroFerrante/Embedded-Systems/blob/main/I2C/MEMS/mems.c): implementation of acellerometer and gyroscope sensor;

## Requirements
- Visual Studio Code with the [PlatformIO](https://platformio.org/).
- `stm32_unict_lib` library provided by the university for the STM32 microcontroller.

## License
This project is licensed under the MIT license. See the `LICENSE` file for more details.

## Authors
**Alessandro Ferrante**