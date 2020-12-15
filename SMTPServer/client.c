#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client.h"

#include "../SMTPShared/shared_strings.h"

server_client_t empty_client() {
    server_client_t client = {
        .client_state = SERVER_FSM_ST_INIT,
        .inp_buf = NULL,
        .out_buf = NULL,
        .inp_len = 0,
        .out_len = 0,
        .mail = empty_mail()
    };

    return client;
}

void reset_client_mail(server_client_t* client) {
    reset_mail(&client->mail);
}

int client_add_from(server_client_t* client, const char* from, int len) {
    // Add len here and in other places to store '\0' too
    return concat_dynamic_strings(&client->mail.from, from, 0, len + 1);
}

int client_add_to(server_client_t* client, const char* to, int len, const char* domain) {
    if (client->mail.to_len >= MAX_TO)
        return 0;

    if (concat_dynamic_strings(&client->mail.to[client->mail.to_len], to, 0, len + 1) < 0)
        return -1; 
    client->mail.to_len++;

    char* to_domain = strstr(to, domain);
    if (to_domain) {
        client->mail.to_type |= LOCAL_MAIL;
    } else {
        client->mail.to_type |= DISTANT_MAIL;
    }

    return 1;
}

int client_add_data(server_client_t* client, const char* data, int len) {
    return concat_dynamic_strings(&client->mail.data, data, 0, len + 1);
}

int client_save_mail(server_client_t* client, const char* maildir, const char* client_mail_dir) {
    if (client->mail.to_type == MAIL_NONE)
        return -1;  // assert?

    int res = 0;

    if (client->mail.to_type & LOCAL_MAIL) {
        res = save_mail(maildir, strlen(maildir), client->mail);
    } 

    if (client->mail.to_type & DISTANT_MAIL && !res) {
        res = save_mail(client_mail_dir, strlen(client_mail_dir), client->mail);
    }

    return res;
}

void client_dict_free(server_client_dict_t** dict) {
    server_client_dict_t *curr, *next;

    for (curr = *dict; curr != NULL; curr = next) {
        next = curr->next;
        
        server_client_t val = curr->value;
        if (val.inp_buf)
            free(val.inp_buf);
        if (val.out_buf)
            free(val.out_buf);
        reset_mail(&val.mail);

        free(curr);
    }   
}

server_client_t* get_item(server_client_dict_t* dict, int key) {
    server_client_dict_t *ptr;

    for (ptr = dict; ptr != NULL; ptr = ptr->next) {
        if (ptr->key == key) {
            return &ptr->value;
        }
    }
    
    return NULL;
}

int del_item(server_client_dict_t** dict, int key) {
    server_client_dict_t *ptr, *prev;

    for (ptr = *dict, prev = NULL; ptr != NULL; prev = ptr, ptr = ptr->next) {
        if (ptr->key == key) {
            if (ptr->next != NULL) {
                if (prev == NULL) {
                    *dict = ptr->next;
                } else {
                    prev->next = ptr->next;
                }
            } else if (prev != NULL) {
                prev->next = NULL;
            } else {
                *dict = NULL;
            }
            
            if (ptr->value.inp_buf)
                free(ptr->value.inp_buf);
            if (ptr->value.out_buf)
                free(ptr->value.out_buf);
            reset_mail(&ptr->value.mail);
            free(ptr);
            
            return 0;
        }
    }

    return -1;
}

int add_item(server_client_dict_t** dict, int key, server_client_t value) {
    if (get_item(*dict, key) != NULL)
        return -1;

    server_client_dict_t *new_dict_head = (server_client_dict_t*) malloc(sizeof(server_client_dict_t));
    if (!new_dict_head)
        return -2;

    new_dict_head->key = key;
    new_dict_head->value = value;
    new_dict_head->next = *dict;
    *dict = new_dict_head;

    return 0;
}