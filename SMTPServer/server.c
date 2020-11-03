#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "pollbuf_dictionary.h"

#include "../SMTPShared/full_io.h"
 
#define BUFSIZE 6       
 
#define POLL_SIZE 1024     
#define POLL_SERVER_IND 0  
#define POLL_WAIT 1000  
 
#define SERVER_PORT 8080  
 
static volatile int is_running = 1;

/// Обработка сигнала SIGINT
void int_handler(int dummy) {
    is_running = 0;
    printf("\n");
}

int add_client(struct pollfd* client_poll_fd, int server_fd, pollbuf_dictionary** buf_dict) {
    struct sockaddr_in sender;           // Адрес клиента
    socklen_t addrlen = sizeof(sender); 
    int client_d;                        // Сокет клиента   
    poll_socket_buf client_buf;          // Буфер клиента для poll'а
    int buf_update_res;                
 
    client_d = accept(server_fd, (struct sockaddr*) &sender, &addrlen);
    if (client_d < 0)
        return client_d;
 
    client_poll_fd->fd = client_d;
    client_poll_fd->events = POLLIN;
    client_poll_fd->revents = 0;

    client_buf.buf = NULL;
    client_buf.len = 0;
    if ((buf_update_res = add_item(buf_dict, client_d, client_buf)) < 0)
        return buf_update_res;
 
    printf("\n>> Connected client %d\n", client_d);
 
    return 0;
}

int send_to_client(int client_fd, char* buf, int len) {
    int send_len;

    if ((send_len = full_send(client_fd, buf, len, 0)) > 0)
        printf("<< Sent back to client %d\n", client_fd);

    return send_len;
}

int serve_client(int client_fd, pollbuf_dictionary* buf_dict) {
    char buf[BUFSIZE]; 
    int len; 
    poll_socket_buf *client_buf = get_item(buf_dict, client_fd);

    len = recv(client_fd, buf, BUFSIZE, 0);
    if (len > 0) {
        int message_begin_index = 0;  
        int message_end_index = -1;

        // Итерируемся по прочитанному и ищем флаги конца передачи
        for (int i = 0; i < len-2; i++) {
            if (buf[i] == EOT[0] && buf[i+1] == EOT[1] && buf[i+2] == EOT[2]) {
                // Как только нашли флаг конца передачи, собираем сообщение и полученных сейчас и ранее
                message_end_index = i+2;

                int concat_res = concat_dynamic_strings_from(&client_buf->buf, buf, client_buf->len, 
                    message_begin_index, message_end_index);
                if (concat_res < 0)
                    return concat_res;

                client_buf->len += message_end_index - message_begin_index + 1;
                client_buf->buf[client_buf->len] = '\0';

                printf("\n>> Recieved \"%s\" from %d\n", client_buf->buf, client_fd);

                send_to_client(client_fd, client_buf->buf, client_buf->len);

                free(client_buf->buf);
                client_buf->buf = NULL;
                client_buf->len = 0;

                i += 2;  // Пропускаем две итерации, т.к. следующие символы '.\n'
                message_begin_index = i + 1;
            }
        }
        
        // Если еще что-то осталось не прочитанное, сохраняем на будущее
        if (message_begin_index <= len - 1) {
            int concat_res = concat_dynamic_strings_from(&client_buf->buf, buf, client_buf->len, 
                message_begin_index, len - 1);
            if (concat_res < 0)
                return concat_res;

            client_buf->len += len - message_begin_index;
        }
    } else if (len == 0) {
        printf("\n>> Client %d closed connection\n", client_fd);

        close(client_fd);

        if (!client_buf->buf) {
            free(client_buf->buf);
            client_buf->buf = NULL;
            client_buf->len = 0;
        }
    }

    return len;
}

int main() {
    int sock_d;                    // Файловый дескриптор сокета сервера
    int fd_max = 0;                // Максимальный номер дескриптора в буфере
    struct pollfd fds[POLL_SIZE];  // Буфер poll'а
    struct sockaddr_in addr;       // Адрес сервера
    pollbuf_dictionary *buf_dict;  // Буфер для сообщений от сокетов в poll'е
    int option = 1;                // Option для SO_REUSEADDR
    int poll_res; 
 
    signal(SIGINT, int_handler);

    printf("Starting server...\n");
 
    if ((sock_d = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
 
    if (setsockopt(sock_d, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0) {
        perror("setsockopt");
        exit(1);
    }
 
    fds[POLL_SERVER_IND].fd = sock_d;
    fds[POLL_SERVER_IND].events = POLLIN;
    fds[POLL_SERVER_IND].revents = 0;
 
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    addr.sin_port = htons(SERVER_PORT);
 
    if (bind(sock_d, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }
  
    if (listen(sock_d, SOMAXCONN) < 0) {
        perror("listen");
        exit(1);
    };

    printf("Server listening --- press Ctrl-C to stop\n");
    while (is_running) {
        poll_res = poll(fds, fd_max + 1, POLL_WAIT);
 
        if (poll_res < -1) {
            perror("poll");
        } else if (poll_res > 0) {
            if (fds[POLL_SERVER_IND].revents & POLLIN) {
                fds[POLL_SERVER_IND].revents = 0;
 
                fd_max++;
                if (add_client(fds+fd_max, fds[POLL_SERVER_IND].fd, &buf_dict) < 0) {
                    perror("add_client");
                    break;
                }
            }

            for (int i = POLL_SERVER_IND + 1; i <= fd_max; i++) {
                if (fds[i].revents & POLLIN) {
                    fds[i].revents = 0;

                    int res = serve_client(fds[i].fd, buf_dict);

                    // Сдвигаем влево буфер poll'а если клиент удалился
                    // и удаляем запись в таблице буферов сообщений клиентов
                    if (res == 0) {
                        del_item(&buf_dict, fds[i].fd);

                        if (i < fd_max)
                            memcpy(&fds[i], &fds[i + 1], sizeof(struct pollfd) * (fd_max - i));
                        fd_max--;
                    } else if (res < 0) {
                        perror("serve_client");
                        is_running = 0;
                        break;
                    }
                }
            }
        }
    }
 
    printf("Closing all connections...\n");
    for (int i = POLL_SERVER_IND + 1; i <= fd_max; i++) {
        close(fds[i].fd);
    }

    dict_free(&buf_dict);

    close(sock_d);
 
    printf("Server stopped\n");
 
    return 0;
}
 