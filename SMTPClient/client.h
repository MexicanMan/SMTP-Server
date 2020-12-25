#ifndef CLIENT_H
#define CLIENT_H

#include "./dirwork/dir_worker.h"
#include "../SMTPShared/logger/logger.h"

int main_old(int argc, char **argv);
int batch_files_for_processes(mail_files_t* mails, int processes_count, logger_t* logger);
int process_mail_files(mail_files_t* mails, int start_ind, int end_ind);
int process_mails(char* mail_text/*заменить на структурированные письма*/);

#endif