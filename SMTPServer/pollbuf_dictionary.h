#ifndef POLLBUF_DICTIONARY_H
#define POLLBUF_DICTIONARY_H

typedef struct poll_socket_buf_struct {
    char* inp_buf;
    char* out_buf;
    int inp_len;
    int out_len;
} poll_socket_buf;

typedef struct pollbuf_dict_struct {
    int key;
    poll_socket_buf value;
    struct pollbuf_dict_struct *next;
} pollbuf_dictionary;

void dict_free(pollbuf_dictionary** dict);
poll_socket_buf* get_item(pollbuf_dictionary* dict, int key);
int del_item(pollbuf_dictionary** dict, int key);
int add_item(pollbuf_dictionary** dict, int key, poll_socket_buf value);

#endif