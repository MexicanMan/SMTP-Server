#ifndef PARCER_H
#define PARCER_H

typedef struct Mail
{
    char** mail_text;
    char* from;
    char** recievers;
    char** hosts;
    int* ports;
    int tos_count;
    int text_len;
} mail_t;

char** read_file(char* filename, int* str_num);
int copy_str_to_arr(char* str, char** arr, int pos);
char** init_mail_text();
int clear_mail_text(char** arr);
mail_t* parse_mail(char** mail_file_text, int str_num, int is_home_mode);
char** get_recievers_from_hosts(char** hosts, int* ports, int is_home_mode);
char* get_address_from_reciever(char* reciever_host, int* port, int is_home_mode);
char* cut_addresses_from_mail_format(char* addr_str_raw);
char* cut_host_from_reciever(char* reciever);
char* get_record(char* host, int type);
void clear_mail(mail_t* mail);
char* try_parse_message_part(char** buf, int bufsize, int* len, int* new_len);
int parse_return_code(char* buf);

#endif