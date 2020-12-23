#ifndef CLIENT_H
#define CLIENT_H

#include "mail.h"

#include "autogen/server-fsm.h"

/**
 * @brief Client struct
 */
typedef struct server_client_struct {
    te_server_fsm_state client_state;   ///< Current client state

    char* inp_buf;                      ///< Client input buffer (read)
    char* out_buf;                      ///< Client output buffer (write)
    int inp_len;
    int out_len;

    mail_t mail;                        ///< Client current mail
} server_client_t;

/**
 * @brief Fd-client dictionary
 */
typedef struct server_client_dict_struct {
	int key;                                    ///< Fd of client
	server_client_t value;                      ///< Client itself

    struct server_client_dict_struct *next;
} server_client_dict_t;

server_client_t empty_client();
void reset_client_mail(server_client_t* client);
int client_add_from(server_client_t* client, const char* from, int len);
int client_add_to(server_client_t* client, const char* to, int len, const char* domain);
int client_add_data(server_client_t* client, const char* data, int len);
int client_save_mail(server_client_t* client, const char* maildir, const char* client_mail_dir, const char* domain);

void client_dict_free(server_client_dict_t** dict);
server_client_t* get_client_by_key(server_client_dict_t* dict, int key);
int del_client_by_key(server_client_dict_t** dict, int key);
int add_client_by_key(server_client_dict_t** dict, int key, server_client_t value);

#endif