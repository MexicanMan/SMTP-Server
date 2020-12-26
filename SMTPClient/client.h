#ifndef CLIENT_H
#define CLIENT_H

#include "./dirwork/dir_worker.h"
#include "./smtp/parser.h"
#include "../SMTPShared/logger/logger.h"

int batch_files_for_processes(mail_files_t* mails, int processes_count, logger_t* logger, int is_home_mode);
int process_mail_files(mail_files_t* mails, int start_ind, int end_ind, logger_t* logger, int is_home_mode);
int process_mails(mail_t** mail_text, logger_t* logger);

#endif