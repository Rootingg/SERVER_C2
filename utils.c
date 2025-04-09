#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

char* generate_uuid() {
    char* uuid = (char*)malloc(11 * sizeof(char));
    if (uuid == NULL) return NULL;
    
    static const char hex_chars[] = "0123456789abcdef";
    for (int i = 0; i < 10; i++) {
        uuid[i] = hex_chars[rand() % 16];
    }
    uuid[10] = '\0';
    
    return uuid;
}