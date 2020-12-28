#ifndef FSM_HANDLERS_H
#define FSM_HANDLERS_H

#include "./client-fsm.h"
#include "../smtp/smtp_conn.h"

te_client_fsm_state HANDLE_CONNECT(te_client_fsm_state next_state, void* connection, void* writeFS);
te_client_fsm_state HANDLE_EHLO(te_client_fsm_state next_state, void* connection, void* writeFS);
te_client_fsm_state HANDLE_QUIT(te_client_fsm_state next_state, void* connection, void* writeFS);
te_client_fsm_state HANDLE_ERROR(te_client_fsm_state next_state, void* connection, void* writeFS);
te_client_fsm_state HANDLE_MF(te_client_fsm_state next_state, void* connection, void* writeFS);
te_client_fsm_state HANDLE_RT(te_client_fsm_state next_state, void* connection, void* writeFS);
te_client_fsm_state HANDLE_DATA(te_client_fsm_state next_state, void* connection, void* writeFS);
te_client_fsm_state HANDLE_FINISH(te_client_fsm_state next_state, void* connection, void* writeFS);
char* concat_mail_text(char** mail_text, int text_len);

#endif
