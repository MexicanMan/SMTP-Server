#ifndef PARCER_H
#define PARCER_H

typedef struct Mail
{
    char** raw_text;
    char** mail_text;
    char* from;
    char** to;
    char** recievers;
    char** hosts;
} mail_t;

char** read_file(char* filename, int* str_num);
int copy_str_to_arr(char* str, char** arr, int pos);
char** init_mail_text();
int clear_mail_text(char** arr);
mail_t* parse_mail(char** mail_file_text, int str_num, int is_home_mode);
char** get_recievers_from_hosts(char** hosts);
char* get_address_from_reciever(char* reciever_host, int is_home_mode);
char* cut_addresses_from_mail_format(char* addr_str_raw);
char* cut_host_from_reciever(char* reciever);
char* get_record(char* host, int type);

#endif