#ifndef CLIENT_H
#define CLIENT_H

#include "mail.h"

#include "autogen/server-fsm.h"

typedef struct server_client_struct {
    te_server_fsm_state client_state;

    char* inp_buf;
    char* out_buf;
    int inp_len;
    int out_len;

    mail_t mail;
} server_client_t;

typedef struct server_client_dict_struct {
	int key;
	server_client_t value;

    struct server_client_dict_struct *next;
} server_client_dict_t;

server_client_t empty_client();
void reset_client_mail(server_client_t* client);
int client_add_from(server_client_t* client, const char* from, int len);
int client_add_to(server_client_t* client, const char* to, int len);

void client_dict_free(server_client_dict_t** dict);
server_client_t* get_item(server_client_dict_t* dict, int key);
int del_item(server_client_dict_t** dict, int key);
int add_item(server_client_dict_t** dict, int key, server_client_t value);

#endif