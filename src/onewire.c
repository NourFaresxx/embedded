#include "onewire.h"

// Reset function: Sends a reset pulse to the OneWire bus
bool onewire_reset() {
    gpio_set_dir(ONE_WIRE_PIN, GPIO_OUT);
    gpio_put(ONE_WIRE_PIN, 0);  // Pull low
    sleep_us(480);              // Wait for 480µs
    gpio_put(ONE_WIRE_PIN, 1);  // Release line
    gpio_set_dir(ONE_WIRE_PIN, GPIO_IN); // Switch to input mode
    sleep_us(60);               // Wait for 60µs
    bool presence = !gpio_get(ONE_WIRE_PIN);  // Presence pulse from sensor
    sleep_us(420);              // Wait for 420µs before next operation
    return presence;
}

// Write a byte to the OneWire bus
void onewire_write_byte(uint8_t byte) {
    gpio_set_dir(ONE_WIRE_PIN, GPIO_OUT);
    for (int i = 0; i < 8; i++) {
        gpio_put(ONE_WIRE_PIN, 0); // Pull low
        sleep_us(1);
        gpio_put(ONE_WIRE_PIN, (byte & (1 << i)) ? 1 : 0); // Write bit
        sleep_us(60);
        gpio_put(ONE_WIRE_PIN, 1); // Release line
        sleep_us(60);
    }
}

// Read a byte from the OneWire bus
uint8_t onewire_read_byte() {
    uint8_t value = 0;
    for (int i = 0; i < 8; i++) {
        gpio_set_dir(ONE_WIRE_PIN, GPIO_OUT);
        gpio_put(ONE_WIRE_PIN, 0); // Pull low
        sleep_us(1);
        gpio_set_dir(ONE_WIRE_PIN, GPIO_IN); // Release line
        sleep_us(14);
        if (gpio_get(ONE_WIRE_PIN)) {
            value |= (1 << i); // If line is high, set bit
        }
        sleep_us(45);
    }
    return value;
}
