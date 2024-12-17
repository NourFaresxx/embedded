#ifndef SERVO_H_
#define SERVO_H_

#include "pico/stdlib.h"

// Servo structure definition
typedef struct {
    uint8_t gpio_pin;
} Servo;

// Initializes the servo
void Servo_init(Servo *servo, uint8_t gp);

// Moves the servo to the specified angle (0 to 180 degrees)
void Servo_goDegree(Servo *servo, float degree);

#endif /* SERVO_H_ */
