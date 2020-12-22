#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "server.h"
#include "commands_parser.h"

#include "autogen/server-fsm.h"

#include "../SMTPShared/shared_strings.h"

#define POLL_WAIT 1000
#define READBUF_SIZE 20
#define CLIENT_TIMEOUT 300

int server_create_and_biding(const char* address, int port);
int server_process_pollfds(server_t* server);
int server_add_client(server_t* server);
int server_serve_client(server_t* server, int client_ind);
int server_send_client(server_t* server, int client_ind);
int server_lost_client(server_t* server, int client_ind);

/**
 * @brief Initialize smtp server
 * @param logger Logger
 * @param port Port
 * @param mail_dir Directory with local mails
 * @param exit_pipefd Pipe fd for graceful exit 
 * @return Server
 */
server_t* server_init(logger_t* logger, const char* address, int port, const char* domain, 
                      const char* maildir, const char* client_mail_dir, int exit_pipefd) {
    int sock_d;
    
    logger_log(logger, INFO_LOG, "Initializing server...");

    if (parser_initalize(logger) < 0) {
        logger_log(logger, ERROR_LOG, "server_init parser_initalize");
        return NULL;
    }

    server_t* server = (server_t*) malloc(sizeof(server_t));
    if (!server) {
        logger_log(logger, ERROR_LOG, "server_init malloc");
        parser_finalize();
        return NULL;
    }

    server->logger = logger;

    if ((sock_d = server_create_and_biding(address, port)) < 0) {
        logger_log(logger, ERROR_LOG, "server_init server_create_and_biding");
        free(server);
        parser_finalize();
        return NULL;
    }

    server->domain = domain;

    server->fd_max = 0;
    server_fill_pollfd(server, sock_d, POLL_SERVER_IND);
    server_fill_pollfd(server, exit_pipefd, POLL_PIPEFD_IND);
    server->clients = NULL;
    
    server->maildir = maildir;
    server->client_mail_dir = client_mail_dir;

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
            if (server_process_pollfds(server) < 0)
                is_running = 0;
        } else {
            for (int i = POLL_CLIENTS_IND; i < server->fd_max; i++) {
                if (++server->fds_timeouts[i] <= CLIENT_TIMEOUT) 
                    continue;
                
                if (server_lost_client(server, i) < 0) {
                    is_running = 0;
                    break;
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

    parser_finalize();

    logger_log(server->logger, INFO_LOG, "Server stopped");

    free(server);
}

void server_fill_pollfd(server_t* server, int fd, int ind) {
    server->fds[ind].fd = fd;
    server->fds[ind].events = POLLIN;
    server->fds[ind].revents = 0;
    server->fds_timeouts[ind] = 0;

    server->fd_max++;
}

int prepare_send_buf(struct pollfd* client_fd, server_client_t* client, const char* msg, 
                     int len, int is_final) {
    if (concat_dynamic_strings(&client->out_buf, msg, client->out_len, len) < 0)
        return -1;

    client->out_len += len;

    // If it is final - block receive, because client should leave after this message
    if (!is_final)
        client_fd->events |= POLLOUT;
    else
        client_fd->events = POLLOUT;

    return 0;
} 

/**
 * @brief Create and bind server socket
 * @param address Server address
 * @param port Server port
 * @return Server socket or error in cases of < 0 
 */
int server_create_and_biding(const char* address, int port) {
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
    addr.sin_addr.s_addr = inet_addr(address);
    addr.sin_port = htons(port);
 
    if (bind(sock_d, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        close(sock_d);
        return -1;
    }

    return sock_d;
}

int server_process_pollfds(server_t* server) {
    if (server->fds[POLL_PIPEFD_IND].revents & POLLIN) {
        server->fds[POLL_PIPEFD_IND].revents = 0;
        return -1;
    }

    if (server->fds[POLL_SERVER_IND].revents & POLLIN) {
        server->fds[POLL_SERVER_IND].revents = 0;

        if (server_add_client(server) < 0) {
            logger_log(server->logger, ERROR_LOG, "server_process_pollfds server_add_client");
            return -1;
        }
    }

    for (int i = POLL_CLIENTS_IND; i < server->fd_max; i++) {
        if (server->fds[i].revents & POLLIN) {
            server->fds_timeouts[i] = 0;

            if (server_serve_client(server, i) < 0) {
                logger_log(server->logger, ERROR_LOG, "server_process_pollfds server_serve_client");
                return -1;
            }
        }

        if (server->fds[i].revents & POLLOUT) {
            if (server_send_client(server, i) < 0) {
                logger_log(server->logger, ERROR_LOG, "server_process_pollfds server_send_client");
                return -1;
            }
        }

        server->fds[i].revents = 0;
    }

    return 0;
}

int server_add_client(server_t* server) {
    logger_log(server->logger, INFO_LOG, "Accepting new client");

    int client_d = accept(server->fds[POLL_SERVER_IND].fd, NULL, 0);
    if (client_d < 0) {
        logger_log(server->logger, ERROR_LOG, "server_add_client accept");
        return client_d;
    }

    int new_state = server_fsm_step(SERVER_FSM_ST_INIT, SERVER_FSM_EV_CONNECTION_ACCEPTED, client_d, server, NULL, 0);
    if (new_state == SERVER_FSM_ST_SERVER_ERROR) {
        logger_log(server->logger, ERROR_LOG, "server_add_client server_fsm_step");
        return -1;
    }

    logger_log(server->logger, INFO_LOG, "Accepted new client");

    return 0;
}

/**
 * @brief Try to parse probable command or send an error to client
 */
int server_input_command_handle(char* msg, int msg_len, server_t* server, int client_ind, server_client_t* client) {
    int parse_res = commands_parse(msg, msg_len, server, client_ind);
    if (parse_res == 0) {
        if (prepare_send_buf(server->fds + client_ind, client, BAD_CMD_RESP, sizeof(BAD_CMD_RESP), 0) < 0) {
            logger_log(server->logger, ERROR_LOG, "server_input_command_handle prepare_send_buf");
            return -1;
        }
    } else if (parse_res < 0) {
        logger_log(server->logger, ERROR_LOG, "server_input_command_handle commands_parse");
        return -1;
    }

    return 0;
}

/**
 * @brief  Try to find end of line to parse full command
 * @return Length of parsed command or 0 
 */
int server_check_input_command(server_t* server, int client_ind, server_client_t* client) {
    int msg_len = 0;
    char* end = strstr(client->inp_buf, EOL);

    if (end) {
        int end_size = sizeof(EOL) - 1;
        end += end_size;  // strstr returns start of occurence when we need end
        msg_len = end - client->inp_buf;

        if (server_input_command_handle(client->inp_buf, msg_len, server, client_ind, client) < 0) {
            logger_log(server->logger, ERROR_LOG, "server_check_input_command server_input_command_handle");
            return -1;
        }
    }

    return msg_len;
}

/**
 * @brief Try to parse mail
 */
int server_input_mail_handle(char* msg, int msg_len, server_t* server, int client_ind, server_client_t* client) {
    int new_state = server_fsm_step(client->client_state, SERVER_FSM_EV_MAIL_END, client_ind, server, msg, msg_len);
    if (new_state == SERVER_FSM_ST_INVALID) {
        if (prepare_send_buf(server->fds + client_ind, client, BAD_SEQ_RESP, sizeof(BAD_SEQ_RESP), 0) < 0) {
            logger_log(server->logger, ERROR_LOG, "server_input_mail_handle prepare_send_buf");
            return -1;
        }
    } else if (new_state == SERVER_FSM_ST_SERVER_ERROR) {
        logger_log(server->logger, ERROR_LOG, "server_input_mail_handle server_fsm_step");
        return -1;
    }

    return 0;
}

/**
 * @brief  Try to find end of mail to parse full mail
 * @return Length of parsed mail or 0 
 */
int server_check_input_mail(server_t* server, int client_ind, server_client_t* client) {
    int msg_len = 0;
    char* end = strstr(client->inp_buf, EOM);

    if (end) {
        end[0] = EOS;  // We don't need EOM any later

        int end_size = sizeof(EOM) - 1;
        end += end_size; 
        msg_len = end - client->inp_buf;

        if (server_input_mail_handle(client->inp_buf, msg_len - end_size, server, client_ind, client) < 0) {
            logger_log(server->logger, ERROR_LOG, "server_check_input_mail server_input_mail_handle");
            return -1;
        }
    }

    return msg_len;
}

int server_handle_input(server_t* server, int client_ind, server_client_t* client) {
    int msg_len = 0;

    if (client->client_state == SERVER_FSM_ST_INVALID) {
        logger_log(server->logger, ERROR_LOG, "server_handle_input BAD CLIENT STATE");
        return -1;  // assert?
    }

    // If not receiveing DATA
    if (client->client_state != SERVER_FSM_ST_DATA) {
        msg_len = server_check_input_command(server, client_ind, client);
        if (msg_len < 0) {
            logger_log(server->logger, ERROR_LOG, "server_handle_input server_check_input_command");
            return msg_len;
        }
    } else {  // If receiving DATA
        msg_len = server_check_input_mail(server, client_ind, client);
        if (msg_len < 0) {
            logger_log(server->logger, ERROR_LOG, "server_handle_input server_check_input_mail");
            return msg_len;
        }
    }

    if (msg_len) {
        if (msg_len != client->inp_len) {
            client->inp_len -= msg_len;
            memcpy(client->inp_buf, client->inp_buf + msg_len, sizeof(char) * client->inp_len);
        } else {
            free(client->inp_buf);
            client->inp_buf = NULL;
            client->inp_len = 0;
        }
    }

    return 0;
}

int server_serve_client(server_t* server, int client_ind) {
    char buf[READBUF_SIZE]; 

    int client_d = server->fds[client_ind].fd;
    server_client_t* client = get_item(server->clients, client_d);

    int len = recv(client_d, buf, READBUF_SIZE, 0);
    if (len > 0) {
        if (concat_dynamic_strings(&client->inp_buf, buf, client->inp_len, len) < 0) {
            logger_log(server->logger, ERROR_LOG, "server_serve_client concat_dynamic_strings");
            return -1;
        }
        client->inp_len += len;

        if (server_handle_input(server, client_ind, client) < 0) {
            logger_log(server->logger, ERROR_LOG, "server_serve_client server_handle_input");
            return -1;
        }
    } else if (len == 0) {
        server_fsm_step(client->client_state, SERVER_FSM_EV_CONNECTION_LOST, client_ind, server, NULL, 0);
    }

    return len;
}

int server_send_client(server_t* server, int client_ind) {
    int client_d = server->fds[client_ind].fd;
    server_client_t* client = get_item(server->clients, client_d);

    int send_len = send(client_d, client->out_buf, client->out_len, 0);
    if (send_len != client->out_len) {
        client->out_len -= send_len;
        memcpy(client->out_buf, client->out_buf + send_len, sizeof(char) * client->out_len);
    } else {
        free(client->out_buf);
        client->out_buf = NULL;
        client->out_len = 0;

        // If we sent everything, there is no need to keep POLLOUT event
        server->fds[client_ind].events = POLLIN;

        // Check if we sent some client last "goodbye" message
        if (client->client_state == SERVER_FSM_ST_QUIT || client->client_state == SERVER_FSM_ST_TIMEOUT)
            server_fsm_step(client->client_state, SERVER_FSM_EV_CONNECTION_LOST, client_ind, server, NULL, 0);
    }

    return send_len;
}

int server_lost_client(server_t* server, int client_ind) {
    int client_d = server->fds[client_ind].fd;
    server_client_t* client = get_item(server->clients, client_d);

    int new_state = server_fsm_step(client->client_state, SERVER_FSM_EV_CONNECTION_TIMEOUT, client_ind, server, NULL, 0);
    if (new_state == SERVER_FSM_ST_SERVER_ERROR) {
        logger_log(server->logger, ERROR_LOG, "server_lost_client server_fsm_step");
        return -1;
    }

    return 0;
}