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

#include "../SMTPShared/socket_dictionary.h"
#include "../SMTPShared/full_io.h"
 
#define BUFSIZE 2       
 
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

int add_client(struct pollfd* client_poll_fd, int server_fd) {
    struct sockaddr_in sender; 
    socklen_t addrlen = sizeof(sender); 
    int client_d;                       
 
    // Подключение клиента
    client_d = accept(server_fd, (struct sockaddr*) &sender, &addrlen);
    if (client_d < 0)
        return client_d;
 
    client_poll_fd->fd = client_d;
    client_poll_fd->events = POLLIN;
    client_poll_fd->revents = 0;
 
    printf("\n>> Connected client %d\n", client_d);
 
    return 0;
}
 
/*int serve_client(int client_fd) {
    char* buf = NULL; 
    int len;   

    len = full_recv(client_fd, &buf, BUFSIZE, 0);
    if (len > 0) {
        printf("\n>> Recieved \"%s\" from %d\n", buf, client_fd);

        if ((len = full_send(client_fd, buf, len, 0)) > 0)
            printf("<< Sent back to client %d\n", client_fd);
    } else if (len == 0) {
        printf("\n>> Client %d closed connection\n", client_fd);
        close(client_fd);
    }
 
    if (buf) {
        free(buf);
        buf = NULL;
    }

    return len;
}*/

int serve_client(int client_fd) {
    char buf[BUFSIZE]; 
    int len;   

    len = recv(client_fd, buf, BUFSIZE, 0);
    if (len > 0) {
        printf("\n>> Recieved \"%s\" from %d\n", buf, client_fd);

        if ((len = full_send(client_fd, buf, len, 0)) > 0)
            printf("<< Sent back to client %d\n", client_fd);
    } else if (len == 0) {
        printf("\n>> Client %d closed connection\n", client_fd);
        close(client_fd);
    }

    return len;
}

int main() {
    int sock_d;          
    int fd_max = 0;          
    struct pollfd fds[POLL_SIZE];   
    struct sockaddr_in addr;     
    int option = 1;            
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
                if (add_client(fds+fd_max, fds[POLL_SERVER_IND].fd) < 0) {
                    perror("add_client");
                }
            }

            for (int i = POLL_SERVER_IND + 1; i <= fd_max; i++) {
                if (fds[i].revents & POLLIN) {
                    fds[i].revents = 0;
                    int res = serve_client(fds[i].fd);

                    if (res == 0) {
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
 
    printf("Closing all connections...\n");
    for (int i = POLL_SERVER_IND + 1; i <= fd_max; i++) {
        close(fds[i].fd);
    }

    close(sock_d);
 
    printf("Server stopped\n");
 
    return 0;
}
 