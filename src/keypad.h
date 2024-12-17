#ifndef KEYPAD_H
#define KEYPAD_H

#include "pico/stdlib.h"

// Define keypad rows and columns (adjust according to your wiring)
#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4

// Function prototypes
void keypad_init();
char keypad_scan();

#endif // KEYPAD_H
