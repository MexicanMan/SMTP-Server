#ifndef PARCER_H
#define PARCER_H

typedef struct Recievers
{
    char** adresses;
    int count;
} recs_t;

typedef struct Mail
{
    char* text;
    recs_t* recievers;
} mail_t;

char** read_file(char* filename);
int copy_str_to_arr(char* str, char** arr, int pos);
char** init_mail_text();
int clear_mail_text(char** arr);
mail_t* parce_mail(char* mail_file_text);
int fill_recievers_in_mail(mail_t* mail);
recs_t* get_address_from_reciever(char* reciever_adr);

#endif