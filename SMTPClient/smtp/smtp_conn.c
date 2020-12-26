#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "smtp_conn.h"

#define INIT_BUF_SIZE 1024

conn_t* init_connection(mail_t* curr_mail, int to_num)
{
    conn_t* new_conn = malloc(sizeof(conn_t));
    if(new_conn == NULL)
    {
        printf("Can't allocate memory for new connection\n");
        return NULL;
    }
    new_conn->from = curr_mail->from;
    new_conn->to = curr_mail->recievers[to_num];
    new_conn->host = curr_mail->hosts[to_num];
    new_conn->port = curr_mail->ports[to_num];
    new_conn->mail = curr_mail;
    new_conn->receive_buf = malloc(sizeof(char) * INIT_BUF_SIZE);
    new_conn->send_buf = malloc(sizeof(char) * INIT_BUF_SIZE);
    new_conn->sended = 0;
}

int connection_start(conn_t* connection)
{
    struct sockaddr_in addr;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) 
    {
        printf("Can't create socket\n");
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(connection->host);
    addr.sin_port = htons(connection->port);

    if (connect(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0) 
    {
        printf("Can't connect socket\n");
        return -1;
    }

    connection->socket = sock;

    return 0;
}

void clear_connection(conn_t* connection)
{
    free(connection->receive_buf);
    free(connection->send_buf);
    free(connection);
    return;
}