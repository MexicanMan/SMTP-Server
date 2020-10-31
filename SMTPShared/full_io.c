#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "full_io.h"

size_t concat_dynamic_strings(char** str1, const char* str2, size_t str1_len, size_t str2_len) {
    size_t extended_len = str1_len + str2_len;
    char* extended_memory = (char*) calloc(extended_len, sizeof(char));
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

size_t full_recv(int fd, char** out_buf, int read_size, int flags) {
    char buf[read_size];   
    int received_flag = 0; 
    int recv_len = 0;       
    size_t total_len = 0;   
    int concat_res = 0;    

    if (*out_buf) {
        free(*out_buf);
        *out_buf = NULL;
    }

    while (!received_flag) {
        recv_len = recv(fd, buf, read_size, flags);

        if (recv_len <= 0) {
            if (*out_buf) {
                free(*out_buf);
                *out_buf = NULL;
            }

            return recv_len;
        }

        // Если последний прочитанный символ - 4 (EOT), 
        // Значит дошли до конца передачи
        if (buf[recv_len - 1] == EOT) {
            received_flag = 1;
        }

        concat_res = concat_dynamic_strings(out_buf, buf, total_len, recv_len);
        if (concat_res < 0) {
            if (*out_buf) {
                free(*out_buf);
                *out_buf = NULL;
            }

            return concat_res;
        }

        total_len += recv_len;
    }

    return total_len-1;
}

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