#include "Servo.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include <stdio.h>

#define ROTATE_0 700  // Rotate to 0° position
#define ROTATE_180 2300

void Servo_init(Servo *servo, uint8_t gp) {
    servo->gpio_pin = gp;

    gpio_init(gp);

    // Set up PWM
    gpio_set_function(gp, GPIO_FUNC_PWM);
    pwm_set_gpio_level(gp, 0);
    uint slice_num = pwm_gpio_to_slice_num(gp);

    // Get clock speed and compute divider for 50 Hz
    uint32_t clk = clock_get_hz(clk_sys);
    uint32_t div = clk / (20000 * 50);

    // Ensure divider is in range
    if (div < 1) {
        div = 1;
    }
    if (div > 255) {
        div = 255;
    }

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, (float)div);

    // Set wrap so the period is 20 ms
    pwm_config_set_wrap(&config, 20000);

    // Load the configuration
    pwm_init(slice_num, &config, false);

    pwm_set_enabled(slice_num, true);
}

void Servo_goDegree(Servo *servo, float degree) {
    if (degree > 180.0 || degree < 0.0) {
        return;
    }

    int duty = (((float)(ROTATE_180 - ROTATE_0) / 180.0) * degree) + ROTATE_0;

    // printf("PWM for %.2f deg is %d duty\n", degree, duty);
    pwm_set_gpio_level(servo->gpio_pin, duty);
}
