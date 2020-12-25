#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>

#include "mail.h"

#include "../SMTPShared/dir_helper.h"

#define FROM_HEADER "X-FROM:"
#define TO_HEADER "X-TO:"
#define MAIL_EXT ".mail"
#define MAILDIR "/Maildir"

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
    struct timeval t;
    gettimeofday(&t, NULL);

    // Change Unix time to miliseconds
    unsigned long int curr_time = (unsigned) t.tv_sec * 1000 + t.tv_usec / 1000;
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
            int user_len = to_domain - username + 1;
            char user[user_len];
            snprintf(user, user_len, "%s", username);

            int user_path_len = user_len + len + sizeof(MAILDIR);
            char user_path[user_path_len], full_user_path[user_path_len];
            snprintf(user_path, user_path_len, "%s/%s", path, user);  // Concat mail dir with user dir first
            snprintf(full_user_path, user_path_len, "%s%s", user_path, MAILDIR);  // Then concat ^ with MAILDIR

            if (!(res = create_dir_if_not_exists(user_path)) && !(res = create_dir_if_not_exists(full_user_path)))
                res = save_mail(full_user_path, user_path_len, mail);
        }
    }

    return res;
}