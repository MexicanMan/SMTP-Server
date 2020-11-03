#ifndef FULL_IO_H
#define FULL_IO_H

#define EOT "\n.\n"
#define EOS '\0'

size_t concat_dynamic_strings(char** str1, const char* str2, size_t str1_len, size_t str2_len);
size_t concat_dynamic_strings_from(char** str1, const char* str2, size_t str1_len, size_t from, size_t to);
size_t full_recv(int fd, char** out_buf, int read_size, int flags);
size_t full_send(int fd, const char* message, size_t len, int flags);

#endif