#ifndef CLIENT_H
#define CLIENT_H

#include "./dirwork/dir_worker.h"
#include "./smtp/parser.h"
#include "./smtp/smtp_conn.h"
#include "../SMTPShared/logger/logger.h"

int batch_files_for_processes(mail_files_t* mails, int processes_count, logger_t* logger, int pipeDescr, int is_home_mode);
int process_mail_files(mail_files_t* mails, int start_ind, int end_ind, logger_t* logger, int pipeDescr, int is_home_mode);
int process_mails(mail_t** mail_text, int mail_count, logger_t* logger, int pipeDescr);
int count_mails_connections(mail_t** mails, int mail_count);
conn_t** init_connections(mail_t** mails, int mail_count, int conns_count, logger_t* logger);
void clear_connections(conn_t** connections, int conns_count);
int connections_start(conn_t** connections, int conns_count, logger_t* logger);
conn_t* get_active_connection(conn_t** connections, int conns_count, int socket, logger_t* logger);
int check_connections_for_finish(conn_t** connections, int conns_count);
int process_conn_read(conn_t* connection, logger_t* logger);
int process_conn_write(conn_t* connection, logger_t* logger);
int conn_read(conn_t* connection);
int conn_write(conn_t* connection);
int set_connections_need_write(conn_t** connections, int conns_count, fd_set* writeFS);
int process_message(char* message);

#endif