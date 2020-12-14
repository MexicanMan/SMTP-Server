#ifndef FSM_HANDLERS_H
#define FSM_HANDLERS_H

#include "server.h"
#include "client.h"

#include "autogen/server-fsm.h"

int prepare_send_buf(struct pollfd* client_fd, server_client_t* client, char* msg, int len);

te_server_fsm_state HANDLE_ACCEPTED(server_t* server, int client_d, te_server_fsm_state nextState);
te_server_fsm_state HANDLE_HELO(server_t* server, int client_ind, te_server_fsm_state nextState);
te_server_fsm_state HANDLE_EHLO(server_t* server, int client_ind, te_server_fsm_state nextState);
te_server_fsm_state HANDLE_QUIT(server_t* server, int client_ind, te_server_fsm_state nextState);
te_server_fsm_state HANDLE_VRFY(server_t* server, int client_ind, te_server_fsm_state nextState);
te_server_fsm_state HANDLE_RSET(server_t* server, int client_ind, te_server_fsm_state nextState);
te_server_fsm_state HANDLE_CLOSE(server_t* server, int client_ind, te_server_fsm_state nextState);

#endif