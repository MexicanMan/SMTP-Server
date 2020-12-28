#ifndef SMTP_CONN_H
#define SMTP_CONN_H

#include "./parser.h"
#include "autogen/client-fsm.h"

typedef struct SMTP_Connection
{
    mail_t* mail;
    char* to;
    char* from;
    char* host;
    int port;

    te_client_fsm_state state;

    int socket;
    char* send_buf;
    char* receive_buf;
    int sended;
    int to_send;
    int received;
    int to_receive;
} conn_t;

conn_t* init_connection(mail_t* curr_mail, int to_num);
int connection_start(conn_t* connection);
void clear_connection(conn_t* connection);

#endif