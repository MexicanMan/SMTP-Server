#include "client_fsm_handlers.h"
#include "../smtp/smtp_conn.h"
#include "../../SMTPShared/shared_strings.h"
#include <sys/select.h>
#include <stdlib.h>

#define DOMAIN "arisaka.com"

te_client_fsm_state HANDLE_CONNECT(te_client_fsm_state next_state, void* connection, void* writeFS) 
{
    conn_t* connect = (conn_t*) connection;
    fd_set* set = (fd_set*)writeFS;

    connect->state = next_state;
    char* comm = NULL;
    int new_len = concat_dynamic_strings(&comm, "EHLO ", 0, 5);
    new_len = concat_dynamic_strings(&comm, DOMAIN, new_len, strlen(DOMAIN));
    new_len = concat_dynamic_strings(&comm, "\r\n", new_len, 2);
    new_len = concat_dynamic_strings(&connect->send_buf, comm, connect->to_send, new_len);
    connect->to_send = new_len;
    free(comm);
    FD_SET(connect->socket, set);

    return next_state;
}

te_client_fsm_state HANDLE_EHLO(te_client_fsm_state next_state, void* connection, void* writeFS) 
{
    conn_t* connect = (conn_t*) connection;
    fd_set* set = (fd_set*)writeFS;

    connect->state = next_state;
    char* comm = NULL;
    int new_len = concat_dynamic_strings(&comm, "MAIL FROM:<", 0, 11);
    new_len = concat_dynamic_strings(&comm, connect->from, new_len, strlen(connect->from));
    new_len = concat_dynamic_strings(&comm, ">\r\n", new_len, 3);
    new_len = concat_dynamic_strings(&connect->send_buf, comm, connect->to_send, new_len);
    connect->to_send = new_len;
    free(comm);
    FD_SET(connect->socket, set);

    return next_state;
}

te_client_fsm_state HANDLE_QUIT(te_client_fsm_state next_state, void* connection, void* writeFS) 
{
    conn_t* connect = (conn_t*) connection;
    fd_set* set = (fd_set*)writeFS;

    connect->state = next_state;
    char* comm = NULL;
    int new_len = concat_dynamic_strings(&comm, "QUIT\r\n", 0, 6);
    new_len = concat_dynamic_strings(&connect->send_buf, comm, connect->to_send, new_len);
    connect->to_send = new_len;
    free(comm);
    FD_SET(connect->socket, set);
    
    return next_state;
}

te_client_fsm_state HANDLE_FINISH(te_client_fsm_state next_state, void* connection, void* writeFS) 
{
    conn_t* connect = (conn_t*) connection;
    fd_set* set = (fd_set*)writeFS;

    connect->state = next_state;
    
    return next_state;
}

te_client_fsm_state HANDLE_ERROR(te_client_fsm_state next_state, void* connection, void* writeFS) 
{
    conn_t* connect = (conn_t*) connection;
    fd_set* set = (fd_set*)writeFS;

    connect->state = next_state;

    return next_state;
}

te_client_fsm_state HANDLE_MF(te_client_fsm_state next_state, void* connection, void* writeFS) 
{
    conn_t* connect = (conn_t*) connection;
    fd_set* set = (fd_set*)writeFS;

    connect->state = next_state;
    char* comm = NULL;
    int new_len = concat_dynamic_strings(&comm, "RCPT TO:<", 0, 9);
    new_len = concat_dynamic_strings(&comm, connect->to, new_len, strlen(connect->to));
    new_len = concat_dynamic_strings(&comm, ">\r\n", new_len, 3);
    new_len = concat_dynamic_strings(&connect->send_buf, comm, connect->to_send, new_len);
    connect->to_send = new_len;
    free(comm);
    FD_SET(connect->socket, set);

    return next_state;
}

te_client_fsm_state HANDLE_RT(te_client_fsm_state next_state, void* connection, void* writeFS) 
{
    conn_t* connect = (conn_t*) connection;
    fd_set* set = (fd_set*)writeFS;

    connect->state = next_state;
    char* comm = NULL;
    int new_len = concat_dynamic_strings(&comm, "DATA", 0, 4);
    new_len = concat_dynamic_strings(&comm, "\r\n", new_len, 2);
    new_len = concat_dynamic_strings(&connect->send_buf, comm, connect->to_send, new_len);
    connect->to_send = new_len;
    free(comm);
    FD_SET(connect->socket, set);

    return next_state;
}

te_client_fsm_state HANDLE_DATA(te_client_fsm_state next_state, void* connection, void* writeFS) 
{
    conn_t* connect = (conn_t*) connection;
    fd_set* set = (fd_set*)writeFS;

    connect->state = next_state;
    char* comm = NULL;
    char* text = concat_mail_text(connect->mail->mail_text, connect->mail->text_len);
    if(text == NULL)
    {
        printf("Can't allocate data for mail text to send\n");
        te_client_fsm_state state = HANDLE_ERROR(CLIENT_FSM_ST_S_ERROR, connection, writeFS);
        return state;
    }
    int new_len = concat_dynamic_strings(&comm, text, 0, strlen(text));
    new_len = concat_dynamic_strings(&comm, "\r\n.\r\n", new_len, 5);
    new_len = concat_dynamic_strings(&connect->send_buf, comm, connect->to_send, new_len);
    connect->to_send = new_len;
    free(text);
    free(comm);
    FD_SET(connect->socket, set);

    return next_state;
}

char* concat_mail_text(char** mail_text, int text_len)
{
    char* text = NULL;
    int new_len = 0;
    for(int i = 0; i < text_len; i++)
    {
        new_len = concat_dynamic_strings(&text, mail_text[i], new_len, strlen(mail_text[i]));
    }
    return text;
}
