#ifndef MAIL_H
#define MAIL_H

#define MAX_TO 16

// Mail type flags
#define MAIL_NONE 0x00       // None
#define LOCAL_MAIL 0x01      // Mail for local user (of server domain)
#define DISTANT_MAIL 0x02    // Mail for distant user

typedef struct mail_struct {
    char* from;

    char* to[MAX_TO];
    int to_len;
    int to_type;

    char* data;
} mail_t;

mail_t empty_mail();
void reset_mail(mail_t* mail);
int save_mail(const char* path, int len, mail_t mail);

#endif