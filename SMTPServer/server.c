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

#include "../SMTPShared/full_io.h"
 
#define BUFSIZE 2        // Размер буффера для чтения/записи
 
#define POLL_SIZE 1024     // Размер буффера poll'а 
#define POLL_SERVER_IND 0  // Индекс сокета сервера в буффере poll'а
#define POLL_WAIT 1000     // Время ожидания poll'а
 
#define SERVER_PORT 8080   // Порт сервера
 
static volatile int is_running = 1;
 
/// Обработка сигнала SIGINT
void int_handler(int dummy) {
    is_running = 0;
    printf("\n");
}
 
/// Добавление клиента
/// @param client_poll_fd poll дескриптор сокета клиента
/// @param server_fd Дескриптор сокета сервера
/// @return Результат обработки клиента: 
///  * > 0 - ок,
///  * < 0 - ошибка добавления
int add_client(struct pollfd* client_poll_fd, int server_fd) {
    struct sockaddr_in sender;           // Адрес клиента
    socklen_t addrlen = sizeof(sender);  // Размер структуры с адресом
    int client_d;                        // Сокет клиента   
 
    // Подключение клиента
    client_d = accept(server_fd, (struct sockaddr*) &sender, &addrlen);
    if (client_d < 0)
        return client_d;
 
    // Добавляем сокет клиента в буффер poll'а
    client_poll_fd->fd = client_d;
    client_poll_fd->events = POLLIN;
    client_poll_fd->revents = 0;
 
    printf("\n>> Connected client %d\n", client_d);
 
    return 0;
}
 
/// Обслуживание одного клиента
/// @param client_fd Дескриптор сокета клиента
/// @return Результат обработки клиента: 
///  * > 0 - просто чтение,
///  * = 0 - клиент отключился,
///  * < 0 - ошибка обработки
int serve_client(int client_fd) {
    char* buf = NULL;  // Буффер чтения/записи
    int len;           // Прочтенная/записанная длина
    int new_len;       // Длина для передачи
 
    // Получение информации с клиента
    len = full_recv(client_fd, &buf, BUFSIZE, 0);
    if (len > 0) {
        printf("\n>> Recieved \"%s\" from %d\n", buf, client_fd);

        // Для отправки необходимо в конце добавить символ конца передачи (EOT) и \0
        new_len = len + 1;
        buf = (char*) realloc(buf, new_len);
        buf[len] = EOT;
        buf[new_len] = EOS;

        // Отправка эхо-ответа клиенту
        if ((len = full_send(client_fd, buf, new_len, 0)) > 0)
            printf("<< Sent back to client %d\n", client_fd);
    } else if (len == 0) {
        // Закрываем соединение с клиентом
        printf("\n>> Client %d closed connection\n", client_fd);
        close(client_fd);
    }
 
    if (buf) {
        free(buf);
        buf = NULL;
    }

    return len;
}

int main() {
    int sock_d;                      // Файловый дескриптор сокета
    int fd_max = 0;                  // Максимальный номер дескриптора
    struct pollfd fds[POLL_SIZE];    // Буффер poll'а
    struct sockaddr_in addr;         // Адрес сервера
    int option = 1;                  // Option для SO_REUSEADDR
    int poll_res;                    // Успешность poll'а
 
    // Вешаем обработчик на SIGINT для корректного завершения сервера
    signal(SIGINT, int_handler);
 
    printf("Starting server...\n");
 
    // Создаем TCP-сокет
    if ((sock_d = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
 
    // Добавляем сокету опцию SO_REUSEADDR для быстрого bind'а
    if (setsockopt(sock_d, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0) {
        perror("setsockopt");
        exit(1);
    }
 
    // Добавляем слушающий сокет сервера в начало буффера
    fds[POLL_SERVER_IND].fd = sock_d;
    fds[POLL_SERVER_IND].events = POLLIN;
    fds[POLL_SERVER_IND].revents = 0;
 
    // Указываем адрес и порт сокета сервера
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    addr.sin_port = htons(SERVER_PORT);
 
    // Связываем сокет с адресом и портом 
    if (bind(sock_d, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }
 
    // Переводим сокет в состояние ожидания соединений 
    if (listen(sock_d, SOMAXCONN) < 0) {
        perror("listen");
        exit(1);
    };
 
    printf("Server listening --- press Ctrl-C to stop\n");
    while (is_running) {
        // Опрашиваем сокеты
        poll_res = poll(fds, fd_max + 1, POLL_WAIT);
 
        if (poll_res < -1) {
            perror("poll");
        } else if (poll_res > 0) {
            // Обработка сокета сервера, если пришел новый клиент
            if (fds[POLL_SERVER_IND].revents & POLLIN) {
                fds[POLL_SERVER_IND].revents = 0;
 
                fd_max++;
                if (add_client(fds+fd_max, fds[POLL_SERVER_IND].fd) < 0) {
                    perror("add_client");
                }
            }
 
            // Обработка сокетов клиентов, есл они что-то прислали
            for (int i = POLL_SERVER_IND + 1; i <= fd_max; i++) {
                if (fds[i].revents & POLLIN) {
                    fds[i].revents = 0;
                    int res = serve_client(fds[i].fd);

                    if (res == 0) {
                        // В случае если клиент удалился, смещаем весь буффер влево
                        if (i < fd_max)
                            memcpy(&fds[i], &fds[i + 1], sizeof(struct pollfd) * (fd_max - i));
                        fd_max--;
                    } else if (res < 0) {
                        perror("serve_client");
                    }
                }
            }
        }
    }
 
    // Закрываем все активные сокеты
    printf("Closing all connections...\n");
    for (int i = POLL_SERVER_IND + 1; i <= fd_max; i++) {
        close(fds[i].fd);
    }

    // Закрываем сокет сервера
    close(sock_d);
 
    printf("Server stopped\n");
 
    return 0;
}
 