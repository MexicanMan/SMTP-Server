#ifndef MAIL_H
#define MAIL_H

#define MAX_TO 16

typedef struct mail_struct {
    char* from;

    char* to[MAX_TO];
    int to_len;

    char* data;
} mail_t;

mail_t empty_mail();
void reset_mail(mail_t* mail);

#endif