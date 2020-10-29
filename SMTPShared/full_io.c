#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "full_io.h"

/// Конкатенация двух строк с перевыделением памяти
/// @param str1 Первая строка, куда записывается сообщение
/// @param str2 Вторая строка, откуда копируется сообщение
/// @param str1_len Длина первой строки
/// @param str2_len Длина второй строки
/// @return Длина конкатенированного сообщения или ошибка, если < 0
size_t concat_dynamic_strings(char** str1, const char* str2, size_t str1_len, size_t str2_len) {
    // Выделяем память под текущее сообщение и прочитанное
    size_t extended_len = str1_len + str2_len;
    char* extended_memory = (char*) calloc(extended_len, sizeof(char));
    if (!extended_memory) {
        if (*str1)
            free(*str1);
            
        return -1;
    }

    // Копируем текущее сообщение и прочитанное в новую память
    memcpy(extended_memory, *str1, sizeof(char) * str1_len);
    memcpy(extended_memory + str1_len, str2, sizeof(char) * str2_len);

    // Чистим старую память и переустанавливаем указатель сообщения на новый
    if (*str1)
        free(*str1);
    *str1 = extended_memory;

    return extended_len;
}

/// Полное получение сообщения из сокета с помощью recv
/// @param fd Дескриптор сокета, откуда читается сообщение
/// @param out_buf Выходной буфер с прочитанным сообщением
/// @param read_size Длина чтения сообщения за раз
/// @param flags Флаги для чтения из сокета
/// @return Длина полученного сообщения или ошибка, если <= 0
size_t full_recv(int fd, char** out_buf, int read_size, int flags) {
    char buf[read_size];    // Буфер чтения из сокета
    int received_flag = 0;  // Флаг выхода из цикла чтения
    int recv_len = 0;       // Количество прочитанных символов с помощью recv
    size_t total_len = 0;   // Сколько уже прочитано
    int concat_res = 0;     // Результат конкатенации строк

    if (*out_buf) {
        free(*out_buf);
        *out_buf = NULL;
    }

    while (!received_flag) {
        // Читаем из сокета
        recv_len = recv(fd, buf, read_size, flags);

        if (recv_len <= 0) {
            if (*out_buf) {
                free(*out_buf);
                *out_buf = NULL;
            }

            return recv_len;
        }

        // Если последний прочитанный символ - 4 (EOT), 
        // Значит дошли до конца передачи и заменяем его на \0 (EOS)
        if (buf[recv_len - 1] == EOT) {
            buf[recv_len - 1] = EOS;
            received_flag = 1;
        }

        // Конкатенируем текущую строку и буфер
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

/// Полная отправка сообщения с помощью send
/// @param fd Десктриптор сокета
/// @param message Сообщение для отправки
/// @param len Длинна отправляемого сообщения
/// @param flags Флаги для send
/// @return Длина отправленного сообщения или ошибка, если <= 0
size_t full_send(int fd, const char* message, size_t len, int flags) {
    size_t sent = 0;       // Число отправленных байт
    size_t curr_sent = 0;  // Число отправленых байт на текущей итерации

    while (sent != len) {
        // Отправляем сообщение
        curr_sent = send(fd, message + sent, len - sent, flags);
        if (curr_sent < 0)
            return curr_sent;

        // Увеличиваем общее число отправленных байт
        sent += curr_sent;
    }

    return sent;
}