#ifndef PARCER_H
#define PARCER_H

typedef struct Recievers
{
    char** adresses;
    int count;
} recs_t;

typedef struct Mail
{
    char** raw_text;
    char** mail_text;
    char* from;
    char* to;
    recs_t* recievers;
} mail_t;

char** read_file(char* filename, int* str_num);
int copy_str_to_arr(char* str, char** arr, int pos);
char** init_mail_text();
int clear_mail_text(char** arr);
mail_t* parse_mail(char** mail_file_text, int str_num);
char** fill_recievers_in_mail(char** tos);
recs_t* get_address_from_reciever(char* reciever_adr);
char* cut_addresses_from_mail_format(char* addr_str_raw);

#endif