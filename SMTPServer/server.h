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

    const char* domain;             // Server domain  

    int fd_max;                     // Maximum descriptor number in buffer
    struct pollfd fds[POLL_SIZE];   // Poll buffer
    int fds_timeouts[POLL_SIZE];    // Timeouts for clients

    server_client_dict_t* clients;  // Clients list 

    const char* maildir;            // Maildir
    const char* client_mail_dir;    // Dir with mails to client
} server_t;

server_t* server_init(logger_t* logger, const char* address, int port, const char* domain, 
                      const char* mail_dir, const char* client_mail_dir, int exit_pipefd);
int server_main(server_t* server);
void server_finalize(server_t* server);

void server_fill_pollfd(server_t* server, int fd, int ind);
int prepare_send_buf(struct pollfd* client_fd, server_client_t* client, const char* msg, 
                     int len, int is_final);

#endif