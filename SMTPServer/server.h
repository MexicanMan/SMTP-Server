#ifndef SERVER_H
#define SERVER_H

#include <sys/poll.h>
#include <stdio.h>
#include <stdlib.h>

#include "client.h"

#include "../SMTPShared/logger/logger.h"

#define POLL_SIZE 1024     
#define POLL_SERVER_IND 0
#define POLL_PIPEFD_IND 1
#define POLL_CLIENTS_IND 2  

typedef struct server_struct {
    logger_t* logger;               // Logger        

    int fd_max;                     // Maximum descriptor number in buffer
    struct pollfd fds[POLL_SIZE];   // Poll buffer

    server_client_dict_t* clients;   // Clients list 
} server_t;

server_t* server_init(logger_t* logger, int port, const char* mail_dir, int exit_pipefd);
int server_main(server_t* server);
void server_finalize(server_t* server);

void server_fill_pollfd(server_t* server, int fd, int ind);

#endif