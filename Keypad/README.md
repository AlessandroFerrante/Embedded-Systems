# Keypad

- **[Keypad v1](https://github.com/AlessandroFerrante/Embedded-Systems/blob/main/Keypad/keypad1.c): Synchronous scanning**
   - ### Description
        This file implements a 4x4 keyboard with synchronous scanning.
        Each column is activated one at a time, and for each activation all rows are read to determine which key was pressed. Scanning is managed synchronously via polling.

  - ### Operation
      - Columns are configured as `open-drain` outputs, while rows are configured as `pull-up` inputs.
      - A `keypad_read` function waits for a key to be released and then detects its press.
      - Scanning is polled with a 1 ms delay for debounce.
    
    Synchronous scanning can be inefficient if the microcontroller is busy with other operations.
---

- [Keypad v2](https://github.com/AlessandroFerrante/Embedded-Systems/blob/main/Keypad/keypad2.c): Asynchronous scanning with timer
  - ### Description
    This file implements a 4x4 keyboard with asynchronous scanning using a hardware timer (TIM2). The columns are scanned by the timer that periodically triggers the reading of the rows. The use of interrupts makes the management of the keys more efficient than the synchronous version.

  - ### Operation
    - The columns are outputs and the rows are inputs with pull-ups.
    - A hardware timer scans the columns asynchronously.
    - An interrupt detects the pressed keys without having to block the main flow.

    Asynchronous scanning that allows the microcontroller to perform other operations while managing the keyboard.

---

- **​​[Keypad v3](https://github.com/AlessandroFerrante/Embedded-Systems/blob/main/Keypad/keypad3.c): Advanced management with queue**
  - ### Description
    This file implements a 4x4 keyboard with asynchronous scanning and advanced management via a queue. 
    Key presses are stored in a circular queue, allowing efficient processing of incoming keys.

  - ### Operation
    - Column scanning is handled by a hardware timer (TIM4).
    - Key presses are put into a circular queue to be read later.
    - Key state management (pressed/released) is handled via state transitions.

    Uses a queue to store key presses, improving efficiency in handling multiple rapidly pressed keys.
---
- **[Keypad 4](https://github.com/AlessandroFerrante/Embedded-Systems/blob/main/Keypad/keypad4.c): String Automation and Advanced management with queue**
    - ### Description
        This file integrates keypad reading with string construction and processing. It uses a timer (TIM2) to automate saving strings to a queue, ensuring that strings read from the keypad are processed asynchronously.

  - ### Operation
    - String construction: The function` build_string()` reads characters from the keypad (using `keypad_read()`) and concatenates them into a temporary string.
    - String queue: The current string is duplicated and put into a queue (FIFO)using str_enqueue().
    - Timer automation: The TIM2 timer interrupt (TIM2_IRQHandler) handles saving the string to the queue, reading it with str_dequeue(), and printing the final string.
    - Memory management: Strings are dynamically allocated to avoid leaks, and then freed after processing.


## Requirements
- Visual Studio Code with the [PlatformIO](https://platformio.org/).
- `stm32_unict_lib` library provided by the university for the STM32 microcontroller.
- Keypad 4x4.

## License
This repository is licensed under the MIT license. See the `LICENSE` file for more details.

## Author
**Alessandro Ferrante**