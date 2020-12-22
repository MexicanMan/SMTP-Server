#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "mail.h"

#include "../SMTPShared/dir_helper.h"

#define FROM_HEADER "X-FROM:"
#define TO_HEADER "X-TO:"
#define MAIL_EXT ".mail"

mail_t empty_mail() {
    mail_t mail = {
        .from = NULL,
        .to_len = 0,
        .to_type = MAIL_NONE,
        .data = NULL
    };

    for (int i = 0; i < MAX_TO; i++)   
        mail.to[i] = NULL;

    return mail;
}

void reset_mail(mail_t* mail) {
    if (mail->from) 
        free(mail->from);
    mail->from = NULL;
    
    for (int i = 0; i < mail->to_len; i++) {
        if (mail->to[i])
            free(mail->to[i]);
        mail->to[i] = NULL;
    }
    mail->to_len = 0;
    mail->to_type = MAIL_NONE;

    if (mail->data)
        free(mail->data);
    mail->data = NULL;
}

char* get_mail_filename(const char* path, int len) {
    unsigned long int curr_time = (unsigned) time(NULL);
    int time_len = floor(log10(curr_time)) + 1;
    int filename_len = time_len + len + sizeof(MAIL_EXT) + 1;

    char* filename = (char*) calloc(filename_len, sizeof(char));
    if (!filename)
        return NULL;
    
    snprintf(filename, filename_len, "%s/%lu%s", path, curr_time, MAIL_EXT);
    return filename;
}

int save_mail(const char* path, int len, mail_t mail) {
    char* filename = get_mail_filename(path, len);
    if (!filename)
        return -1;

    int res = 0;
    FILE *f = fopen(filename, "w");
    if (f) {
        fprintf(f, "%s %s\n", FROM_HEADER, mail.from);

        for (int i = 0; i < mail.to_len; i++)
            fprintf(f, "%s %s\n", TO_HEADER, mail.to[i]);

        fprintf(f, "\n%s", mail.data);

        fclose(f);
    } else {
        res = -1;
    }

    free(filename);

    return res;
}

int save_local_mail(const char* path, int len, const char* domain, mail_t mail) {
    int res = 0;

    for (int i = 0; i < mail.to_len && !res; i++) {
        char* to_domain = strstr(mail.to[i], domain);
        if (to_domain) {
            char* username = mail.to[i] + 1;  // As first symbol should be <
            int user_len = to_domain - username;  
            int user_path_len = user_len + len + 2;
            char user_path[user_path_len];
            snprintf(user_path, user_path_len, "%s/%s", path, username);

            if (!(res = create_dir_if_not_exists(user_path)))
                res = save_mail(user_path, user_path_len, mail);
        }
    }

    return res;
}