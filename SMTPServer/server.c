#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "server.h"

#include "autogen/server-fsm.h"

#define POLL_WAIT 1000
#define READBUF_SIZE 20

int server_create_and_biding(int port);
int server_add_client(server_t* server);
int server_serve_client(server_t* server, int client_ind);
int server_send_client(server_t* server, int client_ind);

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
    server->clients = NULL;

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
    
                    if (server_add_client(server) < 0) {
                        logger_log(server->logger, ERROR_LOG, "server_main server_add_client");
                        break;
                    }
                }

                for (int i = POLL_CLIENTS_IND; i < server->fd_max; i++) {
                    if (server->fds[i].revents & POLLIN) {
                        if (server_serve_client(server, i) < 0) {
                            logger_log(server->logger, ERROR_LOG, "server_main server_serve_client");
                            is_running = 0;
                            break;
                        }
                    }

                    if (server->fds[i].revents & POLLOUT) {
                        if (server_send_client(server, i) < 0) {
                            logger_log(server->logger, ERROR_LOG, "server_main server_send_client");
                            is_running = 0;
                            break;
                        }
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

    client_dict_free(&server->clients);

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

int server_add_client(server_t* server) {
    logger_log(server->logger, INFO_LOG, "Accepting new client");

    int client_d = accept(server->fds[POLL_SERVER_IND].fd, NULL, 0);
    if (client_d < 0) {
        logger_log(server->logger, ERROR_LOG, "server_add_client accept");
        return client_d;
    }

    int new_state = server_fsm_step(SERVER_FSM_ST_INIT, SERVER_FSM_EV_CONNECTION_ACCEPTED, client_d, 0, server);
    if (new_state == SERVER_FSM_ST_INVALID) {
        logger_log(server->logger, ERROR_LOG, "server_add_client server_fsm_step");
        return -1;
    }

    logger_log(server->logger, INFO_LOG, "Accepted new client");

    return 0;
}

int server_serve_client(server_t* server, int client_ind) {
    char buf[READBUF_SIZE]; 

    int client_d = server->fds[client_ind].fd;
    server_client_t* client = get_item(server->clients, client_d);

    int len = recv(client_d, buf, READBUF_SIZE, 0);
    if (len > 0) {
        // tmp
        buf[len] = '\0';
        printf("%s", buf);
        // ...
    } else if (len == 0) {
        server_fsm_step(client->client_state, SERVER_FSM_EV_CONNECTION_LOST, client_d, client_ind, server);
    }

    return len;
}

int server_send_client(server_t* server, int client_ind) {
    int client_d = server->fds[client_ind].fd;
    server_client_t* client = get_item(server->clients, client_d);

    int send_len = send(client_d, client->out_buf, client->out_len, 0);
    if (send_len != client->out_len) {
        // Смещаем буфер клиента влево на отосланное сообщение и идем сначала
        client->out_len -= send_len;
        memcpy(client->out_buf, client->out_buf + send_len, sizeof(char) * client->out_len);
    } else {
        free(client->out_buf);
        client->out_buf = NULL;
        client->out_len = 0;

        server->fds[client_ind].events = POLLIN;
    }

    return send_len;
}