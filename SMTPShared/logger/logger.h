#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

#define LOG_FILENAME_LEN 50
#define LOG_TEXT_LEN 200

#define LOG_MSG_READY "_log_ready_"
#define LOG_MSG_CLOSE "_log_close_"

// Logger option flags
#define FILE_PRINT 0x01
#define  CONSOLE_PRINT 0x02

typedef enum {
    DEBUG_LOG = 1,
    INFO_LOG = 2,
    WARNING_LOG = 3,
    ERROR_LOG = 4,
    SYSTEM = 100,  // System messages only like ready (Not logged)
} log_msg_type_t;

typedef struct logger_struct {
    int mq_id;

    char filename[LOG_FILENAME_LEN];
    FILE* file;

    unsigned char options;
} logger_t;

typedef struct log_msg_struct {
    long msg_type;
    char msg_text[LOG_TEXT_LEN];
} log_msg_t;

logger_t* logger_init(const char* log_dir, const char* prog_name, unsigned const char logger_options);
int logger_log(logger_t* logger_sender, log_msg_type_t type, const char* message);
void logger_free(logger_t* logger_sender);

#endif