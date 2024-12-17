#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "onewire.h"
#include "semphr.h"
#include "hardware/uart.h"
#include "Servo.h"
#include <string.h>
#include "GPS.h"

#define TEMP_THRESHOLD_HIGH 26.0 // Example high temperature threshold
#define TEMP_THRESHOLD_LOW 10.0  // Example low temperature threshold
#define GPS_UART_ID uart1        // Using UART1 for GPS communication
#define GPS_BAUD_RATE 9600       // Typical baud rate for GPS modules
#define SERVO_PAD 2              // Servo motor GPIO pin

// Semaphore to manage blocking of temperature task when GPS reading is active
SemaphoreHandle_t gps_semaphore;

// Function to initialize UART for GPS communication
void init_uart()
{
    uart_init(GPS_UART_ID, GPS_BAUD_RATE);                // Initialize UART
    gpio_set_function(4, GPIO_FUNC_UART);                 // Set GPIO 4 as UART TX
    gpio_set_function(5, GPIO_FUNC_UART);                 // Set GPIO 5 as UART RX
    // uart_set_format(GPS_UART_ID, 8, 1, UART_PARITY_NONE); // 8 data bits, 1 stop bit, no parity
}

// Function to read and parse GPS NMEA sentences
// void read_gps_data()
// {
//     char gps_buffer[128];
//     int gps_index = 0;

//     // Read until a complete sentence is received or buffer is full
//     while (uart_is_readable(GPS_UART_ID))
//     {
//         char c = uart_getc(GPS_UART_ID);

//         // Look for a complete sentence (indicated by '\n')
//         if (c == '\n')
//         {
//             gps_buffer[gps_index] = '\0'; // Null-terminate the string

//             // Here, you would parse the NMEA sentence (e.g., $GPGGA) to extract latitude and longitude
//             if (strncmp(gps_buffer, "$GPGGA", 6) == 0)
//             {
//                 // Example NMEA sentence: $GPGGA,123456.78,3741.2345,N,12202.3456,W,1,08,1.0,100.0,M,-34.0,M,,*42
//                 float lat, lon;

//                 // Parsing the latitude and longitude (assuming a valid sentence structure)
//                 sscanf(gps_buffer, "$GPGGA,%*f,%f,%*c,%f,%*c", &lat, &lon);
//                 printf("GPS Data: Lat: %.4f, Lon: %.4f\n", lat, lon);
//             }

//             // Reset buffer for next sentence
//             gps_index = 0;
//         }
//         else
//         {
//             // Store characters in buffer until we find a newline
//             if (gps_index < sizeof(gps_buffer) - 1)
//             {
//                 gps_buffer[gps_index++] = c;
//             }
//         }
//     }
// }

float read_temperature()
{

    if (!onewire_reset())
    {
        printf("Sensor not found!\n");
        return -1.0;
    }


    onewire_write_byte(0xCC); // Skip ROM command
    onewire_write_byte(0x44); // Start temperature conversion
    sleep_ms(750);            // Wait for conversion

    if (!onewire_reset())
    {
        // printf("No sensor detected after conversion!\n");
        return -1.0;
    }

    onewire_write_byte(0xCC); // Skip ROM command
    onewire_write_byte(0xBE); // Read Scratchpad command
    uint8_t temp_lsb = onewire_read_byte();
    uint8_t temp_msb = onewire_read_byte();

    // Handle negative temperatures properly if your sensor returns negative values
    int16_t raw_temp = (temp_msb << 8) | temp_lsb;

    return (float)raw_temp / 16.0; // Convert raw temperature to Celsius
}

// Temperature task
void temperature_task(void *pvParameters)
{
    const uint LED_PIN = 16; // Adjust to the correct GPIO for your LED
    const uint BUZZER_PIN = 11;
    const uint MOTOR_PIN = 28;
    Servo servo;

    Servo_init(&servo, SERVO_PAD);
    gpio_init(MOTOR_PIN);
    gpio_set_dir(MOTOR_PIN, GPIO_OUT); // Set LED pin as output

    // Initialize LED and buzzer pins
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT); // Set LED pin as output
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT); // Set buzzer pin as output

    // Initialize one-wire communication pin
    gpio_init(ONE_WIRE_PIN);
    gpio_set_dir(ONE_WIRE_PIN, GPIO_OUT);
    gpio_put(ONE_WIRE_PIN, 1);

    float temperature = 0.0;

    while (true)
    {
        // Block if GPS task is running
        // if (xSemaphoreTake(gps_semaphore, portMAX_DELAY) == pdTRUE)
        if(true)
        {
            temperature = read_temperature();

            if (temperature != -1.0)
            {
                printf("Temperature: %.2f°C\n", temperature);

                // Check if temperature exceeds or falls below thresholds
                if (temperature > TEMP_THRESHOLD_HIGH || temperature < TEMP_THRESHOLD_LOW)
                {

                    // Activate motor to lock wheel
                    gpio_put(MOTOR_PIN, 1); // Turn motor on (lock the wheel)

                    // Blink LED and toggle buzzer
                    Servo_goDegree(&servo, 180.0);   // Ensure the servo moves to a specific position (180 degrees)
                    vTaskDelay(pdMS_TO_TICKS(500));  // Wait for 500ms at full servo angle
                    Servo_goDegree(&servo, 0.0);     // Move servo back to initial position
                    gpio_put(LED_PIN, 1);            // Turn LED on
                    gpio_put(BUZZER_PIN, 1);         // Turn buzzer on
                    vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1000ms

                    gpio_put(LED_PIN, 0);            // Turn LED off
                    gpio_put(BUZZER_PIN, 0);         // Keep buzzer on
                    vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1000ms

                    // Deactivate motor after 5 seconds
                    // gpio_put(MOTOR_PIN, 0); // Turn motor off (unlock the wheel)
                }
                else
                {
                    gpio_put(LED_PIN, 0);    // Turn LED off
                    gpio_put(BUZZER_PIN, 0); // Turn buzzer off
                    gpio_put(MOTOR_PIN, 0);  // Turn motor off (unlock the wheel)
                }
            }
            else
            {
                gpio_put(LED_PIN, 0);    // Ensure LED is off if reading failed
                gpio_put(BUZZER_PIN, 0); // Ensure buzzer is off if reading failed
                gpio_put(MOTOR_PIN, 0);  // Ensure motor is off
            }
        }

        // Delay for a period before the next reading
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1000ms (1 second)
    }
}

// GPS task
void gps_task(void *pvParameters)
{
  while(true){
    Gps_read();
    // Give the semaphore to allow the temperature task to run
    xSemaphoreGive(gps_semaphore);

    // Delay for a period before the next GPS reading
    vTaskDelay(pdMS_TO_TICKS(2000)); // Delay for 2000ms (2 seconds)
  }
}

int main()
{
    // Initialize stdio for debugging (optional)
    stdio_init_all();

    // Initialize UART
    uart_init(GPS_UART_ID, GPS_BAUD_RATE);

    // Set the UART pins
    gpio_set_function(4, GPIO_FUNC_UART);
    gpio_set_function(5, GPIO_FUNC_UART);

    // Configure UART (8 data bits, no parity, 1 stop bit)
    uart_set_format(GPS_UART_ID, 8, 1, UART_PARITY_NONE);

    // Enable UART FIFO
    uart_set_fifo_enabled(GPS_UART_ID, true);

    // Set up UART interrupt
    irq_set_exclusive_handler(UART1_IRQ, Gps_read);
    irq_set_enabled(UART1_IRQ, true);
    uart_set_irq_enables(GPS_UART_ID, true, false);
    // Test if the serial interface is working
    printf("Starting Program...\n");

    // Create the semaphore, initially giving it to allow the temperature task to run
    gps_semaphore = xSemaphoreCreateBinary();

    // Check if semaphore creation failed
    if (gps_semaphore == NULL)
    {
        printf("Failed to create semaphore!\n");
        while (1)
            ; // Halt if semaphore creation fails
    }

    // Initially give the semaphore to allow temperature task to run
    xSemaphoreGive(gps_semaphore);

    // Create FreeRTOS tasks
    xTaskCreate(temperature_task, "Temperature_Task", 256, NULL, 1, NULL);
    // xTaskCreate(gps_task, "GPS_Task", 4096, NULL, 1, NULL);

    // Start the FreeRTOS scheduler
    vTaskStartScheduler();

    // The program should never reach this point
    while (1)
    {
    };
}
