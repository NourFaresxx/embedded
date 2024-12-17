#ifndef GPS_H
#define GPS_H

#include "pico/stdlib.h"
#define MAX_VALUES 100 // Maximum number of values
#define MAX_LENGTH 50  // Maximum length of each value

// Define the GPIO pin for the OneWire communication
#define RX 5
#define TX 4

// Function prototypes
void Gps_read();

#endif // ONEWIRE_H
