#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
// #define MAX_VALUES 100 // Maximum number of values
// #define MAX_LENGTH 50  // Maximum length of each value
char* deepCopyString(const char* str) {
    // Allocate memory for the new string
    size_t length = strlen(str) + 1;  // +1 for the null terminator
    char* newStr = (char*)malloc(length);

    // Check if malloc was successful
    if (newStr == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    // Copy the string into the new allocated memory
    strcpy(newStr, str);

    return newStr;
}
int split_text(const char *input, char values[MAX_VALUES][MAX_LENGTH]) {
    char temp[MAX_LENGTH * MAX_VALUES]; // Temporary buffer to avoid modifying the original string
    strncpy(temp, input, sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0'; // Ensure null-termination
    
    char *token;
    int count = 0;

    // Tokenize the string
    token = strtok(temp, ",");
    while (token != NULL) {
        // Copy each token into the values array
        strncpy(values[count], token, MAX_LENGTH - 1);
        values[count][MAX_LENGTH - 1] = '\0'; // Ensure null-termination
        count++;
        if (count >= MAX_VALUES) {
            printf("Exceeded maximum values limit!\n");
            break;
        }
        token = strtok(NULL, ",");
    }

    return count; // Return the number of extracted values
}

void clean_buffer(char* buffer, int len){
    for(int i = 0; i < len; i++){
        buffer[i] = 0;
    }
}