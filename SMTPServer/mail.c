#include <stdlib.h>

#include "mail.h"

mail_t empty_mail() {
    mail_t mail = {
        .from = NULL,
        .to_len = 0,
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

    if (mail->data)
        free(mail->data);
    mail->data = NULL;
}