#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pollbuf_dictionary.h"

void dict_free(pollbuf_dictionary** dict) {
    pollbuf_dictionary *curr, *next;

    for (curr = *dict; curr != NULL; curr = next) {
        next = curr->next;
        
        poll_socket_buf val = curr->value;
        if (val.inp_buf)
            free(val.inp_buf);
        if (val.out_buf)
            free(val.out_buf);

        free(curr);
    }   
}

poll_socket_buf* get_item(pollbuf_dictionary* dict, int key) {
    pollbuf_dictionary *ptr;

    for (ptr = dict; ptr != NULL; ptr = ptr->next) {
        if (ptr->key == key) {
            return &ptr->value;
        }
    }
    
    return NULL;
}

int del_item(pollbuf_dictionary** dict, int key) {
    pollbuf_dictionary *ptr, *prev;

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

int add_item(pollbuf_dictionary** dict, int key, poll_socket_buf value) {
    if (get_item(*dict, key) != NULL)
        return -1;

    pollbuf_dictionary *new_dict_head = (pollbuf_dictionary*) malloc(sizeof(struct pollbuf_dict_struct));
    if (!new_dict_head)
        return -2;

    new_dict_head->key = key;
    new_dict_head->value = value;
    new_dict_head->next = *dict;
    *dict = new_dict_head;

    return 0;
}
