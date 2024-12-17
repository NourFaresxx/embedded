#ifndef UTILS_H
#define UTILS_H

#include "pico/stdlib.h"

#define MAX_VALUES 100 // Maximum number of values
#define MAX_LENGTH 50  // Maximum length of each value
// Function prototypes
int split_text(const char *input, char values[MAX_VALUES][MAX_LENGTH]);
void clean_buffer(char* buffer, int len);
char* deepCopyString(const char* str);
#endif 
