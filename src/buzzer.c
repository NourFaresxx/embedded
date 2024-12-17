#include <stdio.h>
#include "buzzer.h"
void buzzer_beep()
{
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT); // Set as output

    // Turn the buzzer on
    gpio_put(BUZZER_PIN, 1);
    sleep_ms(500); // Beep for 0.5 seconds

    // Turn the buzzer off
    gpio_put(BUZZER_PIN, 0);
    sleep_ms(500); // Stay off for 0.5 seconds
}