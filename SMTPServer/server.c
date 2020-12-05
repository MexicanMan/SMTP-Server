#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "server.h"

#define POLL_WAIT 1000

int server_create_and_biding(int port);
void server_fill_pollfd(server_t* server, int fd, int ind);

/**
 * @brief Initialize smtp server
 * @param logger Logger
 * @param port Port
 * @param mail_dir Directory with local mails
 * @param exit_pipefd Pipe fd for graceful exit 
 * @return server_t* 
 */
server_t* server_init(logger_t* logger, int port, const char* mail_dir, int exit_pipefd) {
    int sock_d;
    
    logger_log(logger, INFO_LOG, "Initializing server...");

    server_t* server = (server_t*) malloc(sizeof(server_t));
    if (!server) {
        logger_log(logger, ERROR_LOG, "server_init malloc");
        return NULL;
    }

    server->logger= logger;

    if ((sock_d = server_create_and_biding(port)) < 0) {
        logger_log(logger, ERROR_LOG, "server_init server_create_and_biding");
        free(server);
        return NULL;
    }

    server->fd_max = 0;
    server_fill_pollfd(server, sock_d, POLL_SERVER_IND);
    server_fill_pollfd(server, exit_pipefd, POLL_PIPEFD_IND);

    return server;
}

/**
 * @brief Main server loop
 */
int server_main(server_t* server) {
    if (listen(server->fds[POLL_SERVER_IND].fd, SOMAXCONN) < 0) {
        logger_log(server->logger, ERROR_LOG, "server_main listen");
        return -1;
    }

    logger_log(server->logger, INFO_LOG, "Server listening --- press Ctrl-C to stop");

    int is_running = 1;
    while (is_running) {
        int poll_res = poll(server->fds, server->fd_max, POLL_WAIT); 

        if (poll_res < -1) {
            logger_log(server->logger, WARNING_LOG, "server_main poll");
        } else if (poll_res > 0) {
            if (server->fds[POLL_PIPEFD_IND].revents & POLLIN) {
                server->fds[POLL_PIPEFD_IND].revents = 0;
                is_running = 0;
            } else {
                if (server->fds[POLL_SERVER_IND].revents & POLLIN) {
                    server->fds[POLL_SERVER_IND].revents = 0;
    
                    // ...
                }

                for (int i = POLL_CLIENTS_IND; i < server->fd_max; i++) {
                    if (server->fds[i].revents & POLLIN) {
                        // ...
                    }

                    if (server->fds[i].revents & POLLOUT) {
                        // ...
                    }

                    server->fds[i].revents = 0;
                }
            }
        }
    }

    logger_log(server->logger, INFO_LOG, "Server shutting down..");

    return 0;
}

/**
 * @brief Finaize server
 */
void server_finalize(server_t* server) {
    logger_log(server->logger, INFO_LOG, "Closing all connections...");
    for (int i = POLL_CLIENTS_IND; i < server->fd_max; i++) {
        close(server->fds[i].fd);
    }

    close(server->fds[POLL_SERVER_IND].fd);

    logger_log(server->logger, INFO_LOG, "Server stopped");

    free(server);
}

/**
 * @brief Create and bind server socket
 * @param port Server port
 * @return Server socket or error in cases of < 0 
 */
int server_create_and_biding(int port) {
    int sock_d;                 // Server fd
    struct sockaddr_in addr;    // Server address
    int option = 1;             // Option for SO_REUSEADDR

    if ((sock_d = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return sock_d;
    }

    if (setsockopt(sock_d, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0) {
        close(sock_d);
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    addr.sin_port = htons(port);
 
    if (bind(sock_d, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        close(sock_d);
        return -1;
    }

    return sock_d;
}

void server_fill_pollfd(server_t* server, int fd, int ind) {
    server->fds[ind].fd = fd;
    server->fds[ind].events = POLLIN;
    server->fds[ind].revents = 0;

    server->fd_max++;
}