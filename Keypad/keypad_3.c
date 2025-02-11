/**
 * @file keypad_3.h
 * @author  Alessandro Ferrante (github@alessandroferrante.net)
 * @brief 
 * This code implements a 4x4 keyboard with asynchronous scanning and advanced management via a queue. 
 * Key presses are stored in a circular queue, allowing efficient processing of incoming keys.
 * @version 0.1
 * @date 2024-12-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "stm32_unict_lib.h"
#include <stdio.h>

#define QUEUE_SIZE 40
typedef struct {
	int size, head, tail;
	char data[QUEUE_SIZE];
} t_queue;

enum {
	WAIT_KEY_PRESS, //waiting for a key to be pressed
	WAIT_KEY_RELEASE  // waiting for button release
};

struct s_key_state {
    t_queue key_queue;       // queue to manage key presses
    int halt_scan;           // flag to suspend keyboard scanning
    int current_state;       // current state of the state machine (waiting for pressure/release)
    int current_column;      // column currently active for scanning
    char current_key;        // currently detected key
} key_state;


void init_queue(void) 
{	// initializes the key queue
	key_state.key_queue.size = 0;
	key_state.key_queue.head = 0;
	key_state.key_queue.tail = 0;
}

void enqueue(char c)
{	
	// puts the character at the head of the queue if it has space available, otherwise ignores the character
	if (key_state.key_queue.size < QUEUE_SIZE) {
		key_state.key_queue.data[key_state.key_queue.head] = c;
		// incrementa `head` in modo circolare se raggiunge la fine torna all'inizio
		key_state.key_queue.head = (key_state.key_queue.head + 1) % QUEUE_SIZE; 
		key_state.key_queue.size ++;
	}
}

int dequeue(char * c)
{
	key_state.halt_scan = 1; // suspends keyboard scanning
	// pops characters from the queue if it contains at least one element
	if (key_state.key_queue.size > 0) {
		*c = key_state.key_queue.data[key_state.key_queue.tail]; // extracts from the tail position
		// increment `tail` circularly
		key_state.key_queue.tail = (key_state.key_queue.tail + 1) % QUEUE_SIZE;
		key_state.key_queue.size --;
		key_state.halt_scan = 0; // reset the scan
		return 1; // to indicate that an item was successfully checked out
	}
	else {
		key_state.halt_scan = 0;  // reset the scan
		return 0; // indicates that no elements were checked out
	}
}

void activate_col(int col)
{
	GPIO_write(GPIOA, 10, col != 0);
	GPIO_write(GPIOB, 3, col != 1);
	GPIO_write(GPIOB, 5, col != 2);
	GPIO_write(GPIOB, 4, col != 3);
}

void keypad_init(void)
{
	GPIO_init(GPIOA);
	GPIO_init(GPIOB);
	GPIO_init(GPIOC);

	// cols
	// PA10, PB3, PB5, PB4
	GPIO_config_output(GPIOA, 10);
	GPIO_OPEN_DRAIN(GPIOA, 10);
	GPIO_config_output(GPIOB, 3);
	GPIO_OPEN_DRAIN(GPIOB, 3);
	GPIO_config_output(GPIOB, 5);
	GPIO_OPEN_DRAIN(GPIOB, 5);
	GPIO_config_output(GPIOB, 4);
	GPIO_OPEN_DRAIN(GPIOB, 4);

	// rows
	// PB10, PA8, PA9, PC7
	GPIO_config_input(GPIOB, 10);
	GPIO_PULL_UP(GPIOB, 10);
	GPIO_config_input(GPIOA, 8);
	GPIO_PULL_UP(GPIOA, 8);
	GPIO_config_input(GPIOA, 9);
	GPIO_PULL_UP(GPIOA, 9);
	GPIO_config_input(GPIOC, 7);
	GPIO_PULL_UP(GPIOC, 7);

	init_queue();

	key_state.current_column = 0;
	key_state.current_key = 255;
	key_state.current_state = WAIT_KEY_PRESS;
	key_state.halt_scan = 0;
	activate_col(0);

	TIM_init(TIM4);
	TIM_config_timebase(TIM4, 84, 1000);
	TIM_enable_irq(TIM4, IRQ_UPDATE);
	TIM_set(TIM4,0);
	TIM_on(TIM4);

}

char keys[4][4] = { {'1', '2', '3', 'A'},
					{'4', '5', '6', 'B'},
					{'7', '8', '9', 'C'},
					{'*', '0', '#', 'D'}
};

void TIM4_IRQHandler(void)
{
	if (TIM_update_check(TIM4)) {
		if (!key_state.halt_scan) {
			int r0, r1, r2, r3;
			// leggi la colonna corrente
			r0 = GPIO_read(GPIOB, 10);
			r1 = GPIO_read(GPIOA, 8);
			r2 = GPIO_read(GPIOA, 9);
			r3 = GPIO_read(GPIOC, 7);
			if (r0 == 0) {
				key_state.current_key = keys[key_state.current_column][0];
			}
			if (r1 == 0) {
				key_state.current_key = keys[key_state.current_column][1];
			}
			if (r2 == 0) {
				key_state.current_key = keys[key_state.current_column][2];
			}
			if (r3 == 0) {
				key_state.current_key = keys[key_state.current_column][3];
			}
			// attiva la colonna successiva
			++key_state.current_column;
			if (key_state.current_column == 4) {
				// scansione terminata
				if (key_state.current_key != 255) {
					// tasto premuto
					if (key_state.current_state == WAIT_KEY_PRESS) {
						enqueue(key_state.current_key);
						key_state.current_state = WAIT_KEY_RELEASE;
					}
				}
				else {
					// tasto rilasciato
					if (key_state.current_state == WAIT_KEY_RELEASE)
						key_state.current_state = WAIT_KEY_PRESS;
				}
				key_state.current_column = 0;
				key_state.current_key = 255;
			}
			activate_col(key_state.current_column);
		}
		TIM_update_clear(TIM4);
	}
}

char keypad_read(void) {
	char c;
	while (!dequeue(&c)) ;
	return c;
}

void setup() {
	ClockConfig();
	CONSOLE_init();
	printf("Starting\n");
	keypad_init();
	delay_ms(10000);
	printf("Reading\n");
}

void loop() {
	char c = keypad_read();
		printf("Key = %c\n",c);
		//printf("Queue size = %d\n", key_queue.size);
}

int main(void) {
	setup();
	for (;;) {
		loop();
	}
}