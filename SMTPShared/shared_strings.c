#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "shared_strings.h"

size_t concat_dynamic_strings(char** str1, const char* str2, size_t str1_len, size_t str2_len) {
    size_t extended_len = str1_len + str2_len;
    char *extended_memory = (char*) calloc(extended_len, sizeof(char));
    if (!extended_memory) {
        if (*str1)
            free(*str1);
            
        return -1;
    }

    memcpy(extended_memory, *str1, sizeof(char) * str1_len);
    memcpy(extended_memory + str1_len, str2, sizeof(char) * str2_len);

    if (*str1)
        free(*str1);
    *str1 = extended_memory;

    return extended_len;
}

// To del
size_t full_send(int fd, const char* message, size_t len, int flags) {
    size_t sent = 0;  
    size_t curr_sent = 0;  

    while (sent != len) {
        curr_sent = send(fd, message + sent, len - sent, flags);
        if (curr_sent < 0)
            return curr_sent;

        sent += curr_sent;
    }

    return sent;
}