#ifndef FULL_IO_H
#define FULL_IO_H

#define EOT 4
#define EOS '\0'

size_t full_recv(int fd, char** out_buf, int read_size, int flags);
size_t full_send(int fd, const char* message, size_t len, int flags);

#endif