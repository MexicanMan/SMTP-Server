#ifndef SHARED_STRINGS_H
#define SHARED_STRINGS_H
#include<stdlib.h>

size_t concat_dynamic_strings(char** str1, const char* str2, size_t str1_len, size_t str2_len);

// To del
size_t full_send(int fd, const char* message, size_t len, int flags);

#endif