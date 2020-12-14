#include <string.h>
#include <unistd.h>

#include "fsm_handlers.h"
#include "commands.h"

#include "../SMTPShared/shared_strings.h"

te_server_fsm_state HANDLE_ACCEPTED(server_t* server, int client_d, te_server_fsm_state nextState) {
    logger_log(server->logger, INFO_LOG, "Adding new client");
    
    server_fill_pollfd(server, client_d, server->fd_max);

    server_client_t client;
    client.client_state = nextState;
    client.inp_buf = NULL;
    client.out_buf = NULL;
    client.inp_len = 0;
    client.out_len = 0;

    if (prepare_send_buf(server->fds + server->fd_max - 1, &client, READY_RESP, sizeof(READY_RESP)) < 0) {
        logger_log(server->logger, ERROR_LOG, "HANDLE_ACCEPTED prepare_send_buf");
        return SERVER_FSM_ST_INVALID;
    }

    if (add_item(&server->clients, client_d, client) < 0) {
        logger_log(server->logger, ERROR_LOG, "HANDLE_ACCEPTED add_item");
        return SERVER_FSM_ST_INVALID;
    }

    logger_log(server->logger, INFO_LOG, "New client added");

    return nextState;
}

te_server_fsm_state HANDLE_QUIT(server_t* server, int client_ind, te_server_fsm_state nextState) {
    logger_log(server->logger, INFO_LOG, "Client wants to quit");

    int client_d = server->fds[client_ind].fd;
    server_client_t* client = get_item(server->clients, client_d);

    if (prepare_send_buf(server->fds + client_ind, client, QUIT_RESP, sizeof(QUIT_RESP)) < 0) {
        logger_log(server->logger, ERROR_LOG, "HANDLE_QUIT prepare_send_buf");
        return SERVER_FSM_ST_INVALID;
    }

    client->client_state = nextState;

    return nextState;
}

te_server_fsm_state HANDLE_CLOSE(server_t* server, int client_ind, te_server_fsm_state nextState) {
    logger_log(server->logger, INFO_LOG, "Client leaving");

    int client_d = server->fds[client_ind].fd;

    int last_fd_ind = server->fd_max - 1;
    if (client_ind < last_fd_ind)
        memcpy(server->fds + client_ind, server->fds + client_ind + 1, sizeof(struct pollfd) * (last_fd_ind - client_ind));
    server->fd_max = last_fd_ind;

    del_item(&server->clients, client_d);

    close(client_d);

    logger_log(server->logger, INFO_LOG, "Client left");

    return nextState;
}
