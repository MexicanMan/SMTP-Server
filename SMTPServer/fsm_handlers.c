#include <string.h>

#include "fsm_handlers.h"

te_server_fsm_state HANDLE_ACCEPTED(server_t* server, int client_d, te_server_fsm_state nextState) {
    logger_log(server->logger, INFO_LOG, "Adding new client");
    
    server_client_t client;

    client.client_state = nextState;
    client.inp_buf = NULL;
    client.out_buf = NULL;
    client.inp_len = 0;
    client.out_len = 0;
    if (add_item(&server->clients, client_d, client) < 0)
        return SERVER_FSM_ST_INVALID;

    server_fill_pollfd(server, client_d, server->fd_max);

    // Здесь по идее надо отослать клиенту инфу о подключении

    logger_log(server->logger, INFO_LOG, "New client added");

    return nextState;
}

te_server_fsm_state HANDLE_CLOSE(server_t* server, int client_d, int client_ind, te_server_fsm_state nextState) {
    logger_log(server->logger, INFO_LOG, "Client leaving");

    // Здесь по идее надо отослать клиенту инфу о отключении

    int last_item_ind = server->fd_max - 1;
    if (client_ind < last_item_ind)
        memcpy(server->fds + client_ind, server->fds + client_ind + 1, sizeof(struct pollfd) * (last_item_ind - client_ind));
    server->fd_max = last_item_ind;

    del_item(&server->clients, client_d);

    logger_log(server->logger, INFO_LOG, "Client left");

    return nextState;
}
