#include "GPS.h"
#include "hardware/uart.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define EARTH_RADIUS 6371000
#define GPS_THRESHOLD 50.0 // Example high temperature threshold

char sentence_buffer[256];
const char sentence_type[] = "$GPGGA";
int i = 0;
bool is_GPGGA = true;
char *BASE_LAT;
char *BASE_LON;
int CALIBRATION_READINGS = 0;
uint8_t LED_PIN2 = 13;
float deg_to_rad(float deg) {
    return deg * (3.14159265358979323846 / 180.0);
}

float convert_to_decimal_degrees(char *nmeaCoord, char direction) {
    int degrees = 0;
    float minutes = 0.0;

    if (direction == 'N' || direction == 'S') {
        sscanf(nmeaCoord, "%2d%f", &degrees, &minutes); // Latitude has 2 degrees digits
    } else if (direction == 'E' || direction == 'W') {
        sscanf(nmeaCoord, "%3d%f", &degrees, &minutes); // Longitude has 3 degrees digits
    }

    float decimal_degrees = degrees + (minutes / 60.0);
    if (direction == 'S' || direction == 'W') {
        decimal_degrees *= -1.0;
    }

    return decimal_degrees;
}

float calculate_distance(float lat1, float lon1, float lat2, float lon2)
{
    float lat1_rad = deg_to_rad(lat1);
    float lon1_rad = deg_to_rad(lon1);
    float lat2_rad = deg_to_rad(lat2);
    float lon2_rad = deg_to_rad(lon2);

    // Haversine formula
    float delta_lat = lat2_rad - lat1_rad;
    float delta_lon = lon2_rad - lon1_rad;

    float a = sin(delta_lat / 2.0) * sin(delta_lat / 2.0) + cos(lat1_rad) * cos(lat2_rad) * sin(delta_lon / 2.0) * sin(delta_lon / 2.0);
    float c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));

    // Distance in meters
    return EARTH_RADIUS * c;
}

void Gps_read()
{
    gpio_init(LED_PIN2);
    gpio_set_dir(LED_PIN2, GPIO_OUT);
    char data = uart_getc(uart1); // Read one byte
    sentence_buffer[i] = data;
    // printf("Raw data: %s\n", sentence_buffer);
    // detect start of sentence
    if (data == '$')
    {
        clean_buffer(sentence_buffer, 256);
        i = 0;
        sentence_buffer[i] = data;
        is_GPGGA = false; // set the is_GPGGA initially to false
    }
    else if (data == '\n')
    {
        i = 0;
        // terminaten the sentence buffer
        //  sentence_buffer[i] = '\0';
        // if this was a GPGGA sentence
        if (is_GPGGA)
        {
            // printf("Raw GPGGA data: %s\n", sentence_buffer);
            // extract lat and long
            char values[MAX_VALUES][MAX_LENGTH]; // Array to hold
            split_text(sentence_buffer, values);
            char *lat = values[2];
            char *lon = values[4];
             char lat_dir = values[3][0]; // Latitude direction ('N' or 'S')
            char lon_dir = values[5][0]; // Longitude direction ('E' or 'W')
            if (strcmp(lat, "0") == 0 && strcmp(lon, "99.99") == 0)
            {
                printf("GPS Searching for Signal...\n");
            }
            else
            {
                if(CALIBRATION_READINGS < 10){
                    CALIBRATION_READINGS++;
                    printf("GPS Calibrating...\n");
                }
                else{
                    if((*BASE_LAT == '\0' || *BASE_LON == '\0')){
                    BASE_LAT = deepCopyString(lat);
                    BASE_LON = deepCopyString(lon);
                    }

                float current_lat_decimal = convert_to_decimal_degrees(lat, lat_dir);
                float current_lon_decimal = convert_to_decimal_degrees(lon, lon_dir);
                float base_lat_decimal = convert_to_decimal_degrees(BASE_LAT, lat_dir);
                float base_lon_decimal = convert_to_decimal_degrees(BASE_LON, lon_dir);

                printf("Base Lat: %.6f, Base Lon: %.6f\n", base_lat_decimal, base_lon_decimal);
                printf("Current Lat = %.6f & Current Lon = %.6f\n", current_lat_decimal, current_lon_decimal);

                // Calculate the distance between base and current coordinates
                float distance = calculate_distance(base_lat_decimal, base_lon_decimal, current_lat_decimal, current_lon_decimal);
                printf("Distance: %.2f meters\n", distance);
                if (distance > GPS_THRESHOLD){
                    gpio_put(LED_PIN2, 1);            
                    printf("********The  car moved: %.2f meters\n", distance,"**********");
                }
                }
            }
        }
    }
    else
    {
        // check if the sentence buffer contains 6 characters
        if (i == 5)
        {
            // compare the current 6 characters with GPGGA
            if (strcmp(sentence_buffer, sentence_type) != 0)
            {
                is_GPGGA = false;
            }
            else
            {
                is_GPGGA = true;
            }
        }
    }
    i++;
}