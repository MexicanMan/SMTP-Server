#ifndef FSM_HANDLERS_H
#define FSM_HANDLERS_H

#include "server.h"
#include "client.h"

#include "autogen/server-fsm.h"

te_server_fsm_state HANDLE_ACCEPTED(server_t* server, int client_d, te_server_fsm_state nextState);
te_server_fsm_state HANDLE_CLOSE(server_t* server, int client_d, int client_ind, te_server_fsm_state nextState);

#endif