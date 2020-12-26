#include "client_fsm_handlers.h"
#include "../smtp/smtp_conn.h"
#include <sys/select.h>


te_client_fsm_state HANDLE_CONNECT(te_client_fsm_state next_state, void* connection, void* writeFS) 
{
    conn_t* connect = (conn_t*) connection;
    fd_set* set = (fd_set*)writeFS;
    //Подготовить EHLO к отправке
    return next_state;
}

te_client_fsm_state HANDLE_EHLO(te_client_fsm_state next_state, void* connection, void* writeFS) 
{
    conn_t* connect = (conn_t*) connection;
    fd_set* set = (fd_set*)writeFS;
    return next_state;
}

te_client_fsm_state HANDLE_QUIT(te_client_fsm_state next_state, void* connection, void* writeFS) 
{
    conn_t* connect = (conn_t*) connection;
    fd_set* set = (fd_set*)writeFS;
    return next_state;
}

te_client_fsm_state HANDLE_ERROR(te_client_fsm_state next_state, void* connection, void* writeFS) 
{
    conn_t* connect = (conn_t*) connection;
    fd_set* set = (fd_set*)writeFS;
    return next_state;
}

te_client_fsm_state HANDLE_MF(te_client_fsm_state next_state, void* connection, void* writeFS) 
{
    conn_t* connect = (conn_t*) connection;
    fd_set* set = (fd_set*)writeFS;
    return next_state;
}

te_client_fsm_state HANDLE_RT(te_client_fsm_state next_state, void* connection, void* writeFS) 
{
    conn_t* connect = (conn_t*) connection;
    fd_set* set = (fd_set*)writeFS;
    return next_state;
}

te_client_fsm_state HANDLE_DATA(te_client_fsm_state next_state, void* connection, void* writeFS) 
{
    conn_t* connect = (conn_t*) connection;
    fd_set* set = (fd_set*)writeFS;
    return next_state;
}
