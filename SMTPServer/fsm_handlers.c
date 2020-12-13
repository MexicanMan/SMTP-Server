#include <string.h>

#include "fsm_handlers.h"
#include "commands.h"

#include "../SMTPShared/shared_strings.h"

void prepare_send_buf(struct pollfd* client_fd, server_client_t* client, char* msg, int len) {
    concat_dynamic_strings(&client->out_buf, msg, client->out_len, len); 
    client->out_len += len;

    client_fd->events |= POLLOUT;
} 

te_server_fsm_state HANDLE_ACCEPTED(server_t* server, int client_d, te_server_fsm_state nextState) {
    logger_log(server->logger, INFO_LOG, "Adding new client");
    
    server_fill_pollfd(server, client_d, server->fd_max);

    server_client_t client;
    client.client_state = nextState;
    client.inp_buf = NULL;
    client.out_buf = NULL;
    client.inp_len = 0;
    client.out_len = 0;

    prepare_send_buf(server->fds + server->fd_max - 1, &client, READY_RESP, sizeof(READY_RESP));

    if (add_item(&server->clients, client_d, client) < 0)
        return SERVER_FSM_ST_INVALID;

    logger_log(server->logger, INFO_LOG, "New client added");

    return nextState;
}

te_server_fsm_state HANDLE_CLOSE(server_t* server, int client_d, int client_ind, te_server_fsm_state nextState) {
    logger_log(server->logger, INFO_LOG, "Client leaving");

    int last_item_ind = server->fd_max - 1;
    if (client_ind < last_item_ind)
        memcpy(server->fds + client_ind, server->fds + client_ind + 1, sizeof(struct pollfd) * (last_item_ind - client_ind));
    server->fd_max = last_item_ind;

    del_item(&server->clients, client_d);

    logger_log(server->logger, INFO_LOG, "Client left");

    return nextState;
}
