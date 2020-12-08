#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client.h"

void client_dict_free(server_client_dict_t** dict) {
    server_client_dict_t *curr, *next;

    for (curr = *dict; curr != NULL; curr = next) {
        next = curr->next;
        
        server_client_t val = curr->value;
        if (val.inp_buf)
            free(val.inp_buf);
        if (val.out_buf)
            free(val.out_buf);

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