#include "keypad.h"

// Define the GPIO pins for the rows and columns (adjust according to your wiring)
static const uint8_t row_pins[KEYPAD_ROWS] = { 2, 3, 4, 6 };  // Example GPIOs for rows
static const uint8_t col_pins[KEYPAD_COLS] = { 7, 8, 9, 10 };  // Example GPIOs for columns

// Keypad layout (adjust the key characters as per your keypad layout)
static const char keypad_map[KEYPAD_ROWS][KEYPAD_COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// Function to initialize the keypad
void keypad_init() {
    // Initialize row pins as outputs and column pins as inputs with pull-ups
    for (int i = 0; i < KEYPAD_ROWS; i++) {
        gpio_init(row_pins[i]);
        gpio_set_dir(row_pins[i], GPIO_OUT);
        gpio_put(row_pins[i], 1);  // Set rows high initially
    }

    for (int i = 0; i < KEYPAD_COLS; i++) {
        gpio_init(col_pins[i]);
        gpio_set_dir(col_pins[i], GPIO_IN);
        gpio_pull_up(col_pins[i]);  // Enable pull-up resistors for columns
    }
}

// Function to scan the keypad for pressed keys
char keypad_scan() {
    for (int row = 0; row < KEYPAD_ROWS; row++) {
        gpio_put(row_pins[row], 0);  // Set the current row low

        // Check all columns for a pressed key
        for (int col = 0; col < KEYPAD_COLS; col++) {
            if (gpio_get(col_pins[col]) == 0) {  // Key pressed (low signal)
                while (gpio_get(col_pins[col]) == 0);  // Wait for the key to be released
                gpio_put(row_pins[row], 1);  // Set the row back high
                return keypad_map[row][col];  // Return the key character
            }
        }

        gpio_put(row_pins[row], 1);  // Set the row back high
    }
    return 0;  // Return 0 if no key is pressed
}
