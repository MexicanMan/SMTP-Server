#ifndef FSM_HANDLERS_H
#define FSM_HANDLERS_H

#include "server.h"
#include "client.h"

#include "autogen/server-fsm.h"

te_server_fsm_state HANDLE_ACCEPTED(server_t* server, int client_d, te_server_fsm_state next_state);
te_server_fsm_state HANDLE_HELO(server_t* server, int client_ind, te_server_fsm_state next_state);
te_server_fsm_state HANDLE_EHLO(server_t* server, int client_ind, te_server_fsm_state next_state);
te_server_fsm_state HANDLE_MAIL(server_t* server, int client_ind, const char* data, int len, te_server_fsm_state next_state);
te_server_fsm_state HANDLE_RCPT(server_t* server, int client_ind, const char* data, int len, te_server_fsm_state next_state);
te_server_fsm_state HANDLE_DATA(server_t* server, int client_ind, te_server_fsm_state next_state);
te_server_fsm_state HANDLE_MAIL_RECEIVED(server_t* server, int client_ind, 
                                         const char* data, int len, te_server_fsm_state next_state);
te_server_fsm_state HANDLE_QUIT(server_t* server, int client_ind, te_server_fsm_state next_state);
te_server_fsm_state HANDLE_VRFY(server_t* server, int client_ind, te_server_fsm_state next_state);
te_server_fsm_state HANDLE_RSET(server_t* server, int client_ind, te_server_fsm_state next_state);
te_server_fsm_state HANDLE_CLOSE(server_t* server, int client_ind, te_server_fsm_state next_state);

#endif