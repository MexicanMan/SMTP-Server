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
#include <fcntl.h>

#include "pollbuf_dictionary.h"

#include "autogen/server-opts.h"

#include "../SMTPShared/shared_strings.h"
#include "../SMTPShared/error/error.h"
#include "../SMTPShared/logger/logger.h"
 
#define BUFSIZE 4       
 
#define POLL_SIZE 1024     
#define POLL_SERVER_IND 1  
#define POLL_WAIT 1000  

#define SERVER_PORT 8080  
 
#define EOT "\n.\n"
#define EOS '\0'

static int pfd[2];    

/// Обработка сигнала SIGINT
void int_handler(int dummy) {
    if (write(pfd[1], "c", 1) == -1)
        exit_on_error("write");
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

    client_buf.inp_buf = NULL;
    client_buf.out_buf = NULL;
    client_buf.inp_len = 0;
    client_buf.out_len = 0;
    if ((buf_update_res = add_item(buf_dict, client_d, client_buf)) < 0)
        return buf_update_res;
 
    printf("\n>> Connected client %d\n", client_d);
 
    return 0;
}

void reset_inp_buf(poll_socket_buf* buf) {
    free(buf->inp_buf);
    buf->inp_buf = NULL;
    buf->inp_len = 0;
}

void reset_out_buf(poll_socket_buf* buf) {
    free(buf->out_buf);
    buf->out_buf = NULL;
    buf->out_len = 0;
}


int send_to_client(int client_fd, pollbuf_dictionary* buf_dict) {
    int send_len;
    poll_socket_buf *client_buf = get_item(buf_dict, client_fd);

    if ((send_len = send(client_fd, client_buf->out_buf, client_buf->out_len, 0)) > 0)
        printf("<< Sent back to client %d\n", client_fd);

    if (send_len != client_buf->out_len) {
        // Смещаем буфер клиента влево на отосланное сообщение и идем сначала
        client_buf->out_len -= send_len;
        memcpy(client_buf->out_buf, client_buf->out_buf+send_len, 
            sizeof(char) * client_buf->out_len);
    } else {
        reset_out_buf(client_buf);
    }

    return send_len;
}

int serve_client(int client_fd, pollbuf_dictionary* buf_dict) {
    char buf[BUFSIZE]; 
    int len;
    poll_socket_buf *client_buf = get_item(buf_dict, client_fd);

    len = recv(client_fd, buf, BUFSIZE, 0);
    if (len > 0) {
        int concat_res = concat_dynamic_strings(&client_buf->inp_buf, buf, client_buf->inp_len, len);
        if (concat_res < 0)
            return concat_res;
        client_buf->inp_len += len;

        // Итерируемся по всему прочитанному и ищем флаги конца передачи
        for (int i = 0; i < client_buf->inp_len-2; i++) {
            if (client_buf->inp_buf[i] == EOT[0] && client_buf->inp_buf[i+1] == EOT[1] 
                && client_buf->inp_buf[i+2] == EOT[2]) {
                // Как только нашли флаг конца передачи, вырезаем сообщение
                int message_end_index = i+2;

                char *message = client_buf->inp_buf;
                message[message_end_index] = '\0';

                printf("\n>> Recieved \"%s\" from %d\n", message, client_fd);

                message[message_end_index] = EOT[2];

                int msg_len = message_end_index + 1;
                concat_dynamic_strings(&client_buf->out_buf, client_buf->inp_buf, client_buf->out_len, msg_len);
                client_buf->out_len += msg_len;

                if (msg_len != client_buf->inp_len) {
                    // Смещаем буфер клиента влево на прочитанное сообщение и идем сначала
                    client_buf->inp_len -= msg_len;
                    memcpy(client_buf->inp_buf, client_buf->inp_buf+msg_len, 
                        sizeof(char) * client_buf->inp_len);

                    i = -1;
                } else {
                    reset_inp_buf(client_buf);
                }
            }
        }
    } else if (len == 0) {
        printf("\n>> Client %d closed connection\n", client_fd);

        close(client_fd);

        if (client_buf->inp_buf) {
            reset_inp_buf(client_buf);
        }
    }

    return len;
}

int main(int argc, char **argv) {
    int sock_d;                    // Файловый дескриптор сокета сервера
    int fd_max = 1;                // Максимальный номер дескриптора в буфере
    struct pollfd fds[POLL_SIZE];  // Буфер poll'а
    struct sockaddr_in addr;       // Адрес сервера
    pollbuf_dictionary *buf_dict;  // Буфер для сообщений от сокетов в poll'е
    int option = 1;                // Option для SO_REUSEADDR
    int is_running = 1;

    int optct = optionProcess(&serverOptions, argc, argv);
    printf("%ld %s %s\n", OPT_VALUE_PORT, OPT_ARG(LOG_DIR), OPT_ARG(MAIL_DIR));

    if (pipe2(pfd, O_NONBLOCK) == -1)
        exit_on_error("pipe");
    
    signal(SIGINT, int_handler);
    signal(SIGTERM, int_handler);

    logger_t* logger = logger_init(".", CONSOLE_PRINT);
    if (!logger) 
        exit_on_error("logger_init");

    if (logger_log(logger, INFO_LOG, "Starting server...") < 0)
        warn_on_error("logger_log");
 
    if ((sock_d = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
        exit_on_error("socket");
 
    if (setsockopt(sock_d, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0) 
        exit_on_error("setsockopt");
 
    fds[0].fd = pfd[0];
    fds[0].events = POLLIN;
    fds[0].revents = 0;

    fds[POLL_SERVER_IND].fd = sock_d;
    fds[POLL_SERVER_IND].events = POLLIN;
    fds[POLL_SERVER_IND].revents = 0;
 
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    addr.sin_port = htons(SERVER_PORT);
 
    if (bind(sock_d, (struct sockaddr*) &addr, sizeof(addr)) < 0)
        exit_on_error("bind");
  
    if (listen(sock_d, SOMAXCONN) < 0)
        exit_on_error("listen");

    if (logger_log(logger, INFO_LOG, "Server listening --- press Ctrl-C to stop") < 0)
        warn_on_error("logger_log");
    while (is_running) {
        int poll_res = poll(fds, fd_max + 1, POLL_WAIT); 
 
        if (poll_res < -1) {
            warn_on_error("poll");
        } else if (poll_res > 0) {
            if (fds[0].revents & POLLIN) {
                fds[0].revents = 0;
                is_running = 0;
            } else {
                if (fds[POLL_SERVER_IND].revents & POLLIN) {
                    fds[POLL_SERVER_IND].revents = 0;
    
                    fd_max++;
                    if (add_client(fds+fd_max, fds[POLL_SERVER_IND].fd, &buf_dict) < 0) {
                        warn_on_error("add_client");
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
                                memcpy(fds+i, fds+i+1, sizeof(struct pollfd) * (fd_max - i));
                            fd_max--;
                        } else if (res < 0) {
                            warn_on_error("serve_client");
                            is_running = 0;
                            break;
                        }
                    }
                }

                for (pollbuf_dictionary* i = buf_dict; i != NULL; i = i->next) {
                    if (i->value.out_len > 0) {
                        send_to_client(i->key, buf_dict);
                    }
                }
            }
        }
    }
 
    if (logger_log(logger, INFO_LOG, "Closing all connections...") < 0)
        warn_on_error("logger_log");
    for (int i = POLL_SERVER_IND + 1; i <= fd_max; i++) {
        close(fds[i].fd);
    }

    dict_free(&buf_dict);

    close(sock_d);

    close(pfd[0]);
    close(pfd[1]);

    if (logger_log(logger, INFO_LOG, "Server stopped") < 0)
        warn_on_error("logger_log");

    logger_free(logger);
 
    return 0;
}
 