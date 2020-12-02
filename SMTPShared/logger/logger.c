#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>

#include "logger.h"
#include "../error/error.h"

#define TIME_STR_SIZE 9
#define DATE_STR_SIZE 11
#define MIN_TYPE_STR_SIZE 6

void logger_listener_proc(pid_t pid, const char* log_dir, const char* prog_name, unsigned const char logger_options);
logger_t* logger_sender_proc(const char* prog_name, unsigned const char logger_options);
int logger_init_mq(logger_t* logger, const char* prog_name);
int logger_open_file(logger_t* logger, const char* log_dir);
int logger_main(logger_t logger);
void logger_finalize(logger_t* logger);

int get_log_time(char* time_buf, int len);
int get_log_date(char* date_buf, int len);
int get_log_msg_type(char* type_buf, int len, log_msg_type_t type);

/**
 * @brief  Logger initialization   
 * @param  log_dir: Directory for log files
 * @param  prog_name: Name of currently running program
 * @param  logger_options: Options for logger
 * @retval Logger for sending messages or NULL in case of error
 */
logger_t* logger_init(const char* log_dir, const char* prog_name, unsigned const char logger_options) {
    pid_t pid;

    #ifdef DEBUG
        printf("Logger: Starting initialization\n");
    #endif

    if ((pid = fork()) < 0) {
        return NULL;
    } else if (pid == 0) {
        signal(SIGINT, SIG_IGN);
        pid = getpid();

        #ifdef DEBUG
            printf("Logger: Process forked with pid: %d\n", pid);
            printf("Logger: Parent pid: %d\n", getppid());
        #endif

        logger_listener_proc(pid, log_dir, prog_name, logger_options);
    } else {
        logger_t* logger_sender = logger_sender_proc(prog_name, logger_options);
        if (!logger_sender)
            kill(pid, SIGTERM);  // If something goes wrong parent kill child

        return logger_sender;
    }

    // assert ?    
}

/**
 * @brief  Log a message
 * @param  logger_sender: Sender logger
 * @param  type: Message type
 * @param  message: Message 
 * @retval Number of logged symbols or error
 */
int logger_log(logger_t* logger_sender, log_msg_type_t type, const char* message) {
    log_msg_t log_msg;
    int log_msg_sz = sizeof(log_msg.msg_text);

    log_msg.msg_type = type;

    char time_buf[TIME_STR_SIZE];
    if (get_log_time(time_buf, TIME_STR_SIZE) < 0)
        return -1;

    char type_buf[MIN_TYPE_STR_SIZE];
    if (get_log_msg_type(type_buf, MIN_TYPE_STR_SIZE, type) < 0)
        return -1;

    snprintf(log_msg.msg_text, log_msg_sz, "%s | [%s] : %s", time_buf, type_buf, message);
    if (msgsnd(logger_sender->mq_id, &log_msg, log_msg_sz, 0) < 0) {
        warning_on_error("logger_log msgsnd");
        return -1;
    }

    return 0;
}

/**
 * @brief  Clear the sender logger and send message to clear listener logger
 */
void logger_free(logger_t* logger_sender) {
    log_msg_t log_msg;
    log_msg.msg_type = INFO_LOG;
    snprintf(log_msg.msg_text, sizeof(LOG_MSG_CLOSE), LOG_MSG_CLOSE);
    msgsnd(logger_sender->mq_id, &log_msg, sizeof(LOG_MSG_CLOSE), 0);

    free(logger_sender);
}

/**
 * @brief  Listener logger process manager
 * @param  pid: pid of logger process
 * @param  logger_options: Options for logger
 */
void logger_listener_proc(pid_t pid, const char* log_dir, const char* prog_name, unsigned const char logger_options) {
    logger_t logger_listener;
    logger_listener.options = logger_options;

    if (logger_init_mq(&logger_listener, prog_name) < 0)
        exit_on_error("logger_init_mq");

    if (logger_open_file(&logger_listener, log_dir) < 0) {
        logger_finalize(&logger_listener);
        exit_on_error("logger_open_file");
    }

    if (logger_main(logger_listener) < 0) {
        perror("logger_main");
    }

    logger_finalize(&logger_listener);

    kill(pid, SIGTERM);
}

/**
 * @brief  
 * @param  prog_name: 
 * @param  logger_options: 
 * @retval Logger for sending messages or NULL in case of error
 */
logger_t* logger_sender_proc(const char* prog_name, unsigned const char logger_options) {
    logger_t* logger_sender = (logger_t*) malloc(sizeof(logger_t));
    if (!logger_sender)
        return NULL;

    logger_sender->options = logger_options;  // Just double for knowledge

    if (logger_init_mq(logger_sender, prog_name) < 0) {
        free(logger_sender);
        logger_sender = NULL;
    }
    else {
        log_msg_t log_msg;
        int log_msg_sz = sizeof(LOG_MSG_READY);
        
        #ifdef DEBUG
            printf("Logger: Waiting logger to be ready..\n");
        #endif

        // Receive msg from logger that it is ready
        msgrcv(logger_sender->mq_id, &log_msg, log_msg_sz, SYSTEM, 0);
        if (strcmp(log_msg.msg_text, LOG_MSG_READY) != 0) {
            msgctl(logger_sender->mq_id, IPC_RMID, NULL);
            free(logger_sender);
            logger_sender = NULL;
        } else {
            #ifdef DEBUG
                printf("Logger: Ready\n");
            #endif
        }
    }

    return logger_sender;
}

/**
 * @brief  Initialization of message queue for logger
 * @param  logger: Logger
 * @param  prog_name: Name of currently running program
 */
int logger_init_mq(logger_t* logger, const char* prog_name)
{
    #ifdef DEBUG
        printf("Logger: Initializing message queue..\n");
    #endif

    key_t key;
    if ((key = ftok(prog_name, 65)) < 0)
        return key;
    if ((logger->mq_id = msgget(key, 0666 | IPC_CREAT)) < 0) 
        return logger->mq_id;

    #ifdef DEBUG
        printf("Logger: Message queue was initialized\n");
    #endif

    return 0;
}

/**
 * @brief  Get log message type string from enum
 * @param  type_buf: Buffer where type string will be put 
 * @param  len: Buffer len, should be equal to MIN_TYPE_STR_SIZE
 * @param  type: log messge type
 * @retval Operation status
 */
int get_log_msg_type(char* type_buf, int len, log_msg_type_t type) {
    if (len < MIN_TYPE_STR_SIZE) {
        errno = EINVAL;
        return -1;
    }

    switch (type) {
        case DEBUG_LOG:
            sprintf(type_buf, "DEBUG");
            break;
        case INFO_LOG:
            sprintf(type_buf, "INFO");
            break;
        case WARNING_LOG:
            sprintf(type_buf, "WARN");
            break;
        case ERROR_LOG:
            sprintf(type_buf, "ERROR");
            break;    
    }

    return 0;
}

/**
 * @brief  Get current time for log
 * @param  date_buf: Buffer where time will be put 
 * @param  len: Buffer len, should be equal to TIME_STR_SIZE
 * @retval Operation status
 */
int get_log_time(char* time_buf, int len)
{
    if (len != TIME_STR_SIZE) {
        errno = EINVAL;
        return -1;
    }

    time_t curr_time = time(NULL);
    struct tm *tm = localtime(&curr_time);

    strftime(time_buf, len, "%H:%M:%S", tm);

    return 0;
}

/**
 * @brief  Get current date for log
 * @param  date_buf: Buffer where date will be put 
 * @param  len: Buffer len, should be equal to DATE_STR_SIZE
 * @retval Operation status
 */
int get_log_date(char* date_buf, int len)
{
    if (len != DATE_STR_SIZE) {
        errno = EINVAL;
        return -1;
    }

    time_t curr_time = time(NULL);
    struct tm *tm = localtime(&curr_time);

    strftime(date_buf, len, "%Y_%m_%d", tm);

    return 0;
}

/**
 * @brief  Log file creation and/or opening
 * @param  logger: Logger
 * @param  log_dir: Directory with log files
 */
int logger_open_file(logger_t* logger, const char* log_dir)
{
    #ifdef DEBUG
        printf("Logger: Opening log file..\n");
    #endif

    char date_buf[DATE_STR_SIZE];
    if (get_log_date(date_buf, DATE_STR_SIZE) < 0)
        return -1;

    snprintf(logger->filename, sizeof(logger->filename), "%s/%s.log", log_dir, date_buf);

    #ifdef DEBUG
        printf("Logger: Filename is %s\n", logger->filename);
    #endif

    if (!(logger->file = fopen(logger->filename, "a"))) 
        return -1;

    #ifdef DEBUG
        printf("Logger: Log file opened\n");
    #endif

    return 0;
}

/**
 * @brief  Printing log message according to the logger options
 */
void print_log_msg(logger_t logger, log_msg_t log_msg)
{
    if (logger.options & FILE_PRINT) {
        fprintf(logger.file, "%s\n", log_msg.msg_text);
    }
    if (logger.options & CONSOLE_PRINT) {
        printf("%s\n", log_msg.msg_text);
    }
}

/**
 * @brief  Logger main loop
 */
int logger_main(logger_t logger)
{
    #ifdef DEBUG
        printf("Logger: Starting main loop\n");
    #endif

    log_msg_t log_msg;
    int log_msg_sz = sizeof(log_msg.msg_text);

    // Tell parent process we are ready
    log_msg.msg_type = SYSTEM;
    snprintf(log_msg.msg_text, sizeof(LOG_MSG_READY), LOG_MSG_READY);
    if (msgsnd(logger.mq_id, &log_msg, sizeof(LOG_MSG_READY), 0) < 0)
        return -1;

    #ifdef DEBUG
        printf("Logger: Main loop started\n");
    #endif

    int is_logger_running = 1;
    while (is_logger_running) {
        if (msgrcv(logger.mq_id, &log_msg, log_msg_sz, SYSTEM, MSG_EXCEPT) < 0) {
            warning_on_error("logger_main msgrcv");
        }

        if (strcmp(log_msg.msg_text, LOG_MSG_CLOSE) == 0) {
            is_logger_running = 0;

            #ifdef DEBUG
                printf("Logger: Closing logger..\n");
            #endif
        } else {
            print_log_msg(logger, log_msg);
        }
    }

    #ifdef DEBUG
        printf("Logger: Main loop finished\n");
    #endif

    return 0;
}

/**
 * @brief  Clear listener logger
 */
void logger_finalize(logger_t* logger)
{
    #ifdef DEBUG
        printf("Logger: Finalize logger...\n");
    #endif

    if (logger->file)
        fclose(logger->file);

    // Destroy message queue only in listener logger
    msgctl(logger->mq_id, IPC_RMID, NULL);

    #ifdef DEBUG
        printf("Logger: Clear finalized\n");
    #endif
}