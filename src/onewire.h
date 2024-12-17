#ifndef ONEWIRE_H
#define ONEWIRE_H

#include "pico/stdlib.h"

// Define the GPIO pin for the OneWire communication
#define ONE_WIRE_PIN 15

// Function prototypes
bool onewire_reset();
void onewire_write_byte(uint8_t byte);
uint8_t onewire_read_byte();

#endif // ONEWIRE_H
