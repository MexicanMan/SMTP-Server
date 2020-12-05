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

void logger_listener_proc(const char* log_dir, int mq_id, unsigned const char logger_options);
logger_t* logger_sender_proc(int mq_id, unsigned const char logger_options);
int logger_init_mq();
int logger_open_file(logger_t* logger, const char* log_dir);
int logger_main(logger_t logger);
void logger_finalize(logger_t* logger);

void get_log_time(char* time_buf, int len);
void get_log_date(char* date_buf, int len);
void get_log_msg_type(char* type_buf, int len, log_msg_type_t type);

/**
 * @brief  Logger initialization   
 * @param  log_dir: Directory for log files
 * @param  logger_options: Options for logger
 * @retval Logger for sending messages or NULL in case of error
 */
logger_t* logger_init(const char* log_dir, unsigned const char logger_options) {
    pid_t pid;

    #ifdef DEBUG
        printf("Logger: Starting initialization\n");
    #endif

    int mq_id = logger_init_mq();
    if (mq_id < 0)
        return NULL;

    if ((pid = fork()) == 0) {
        signal(SIGINT, SIG_IGN);

        #ifdef DEBUG
            printf("Logger: Process forked with pid: %d\n", getpid());
            printf("Logger: Parent pid: %d\n", getppid());
        #endif

        logger_listener_proc(log_dir, mq_id, logger_options);

        exit(0);
    } else {
        logger_t* logger_sender = logger_sender_proc(mq_id, logger_options);
        if (!logger_sender)
            kill(pid, SIGTERM);  // If something goes wrong parent kill child

        return logger_sender;
    }

    return NULL;  
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
    log_msg.msg_type = type;
    log_msg.msg_payload.type = type;
    snprintf(log_msg.msg_payload.text, LOG_TEXT_LEN, "%s", message);

    if (msgsnd(logger_sender->mq_id, &log_msg, sizeof(log_msg.msg_payload), 0) < 0) {
        warn_on_error("logger_log");
        return -1;
    }

    return 0;
}

/**
 * @brief  Clear the sender logger and send message to clear listener logger
 */
void logger_free(logger_t* logger_sender) {
    if (logger_log(logger_sender, INFO_LOG, LOG_MSG_CLOSE) < 0)
        warn_on_error("logger_free msgsnd");

    free(logger_sender);
}

/**
 * @brief  Listener logger process manager
 * @param  pid: pid of logger process
 * @param  logger_options: Options for logger
 */
void logger_listener_proc(const char* log_dir, int mq_id, unsigned const char logger_options) {
    logger_t logger_listener;
    logger_listener.mq_id = mq_id;
    logger_listener.options = logger_options;

    if (logger_options & FILE_PRINT){
        if (logger_open_file(&logger_listener, log_dir) < 0) {
            logger_finalize(&logger_listener);
            exit_on_error("logger_open_file");
        }
    }

    if (logger_main(logger_listener) < 0) {
        warn_on_error("logger_main");
    }

    logger_finalize(&logger_listener);
}

/**
 * @brief  
 * @param  prog_name: 
 * @param  logger_options: 
 * @retval Logger for sending messages or NULL in case of error
 */
logger_t* logger_sender_proc(int mq_id, unsigned const char logger_options) {
    logger_t* logger_sender = (logger_t*) malloc(sizeof(logger_t));
    if (!logger_sender)
        return NULL;

    logger_sender->options = logger_options;  // Just double for knowledge
    logger_sender->mq_id = mq_id;
    
    #ifdef DEBUG
        printf("Logger: Waiting logger to be ready..\n");
    #endif

    // Receive msg from logger that it is ready
    log_msg_t log_msg;
    msgrcv(logger_sender->mq_id, &log_msg, sizeof(log_msg.msg_payload), SYSTEM, 0);
    if (strcmp(log_msg.msg_payload.text, LOG_MSG_READY) != 0) {
        msgctl(logger_sender->mq_id, IPC_RMID, NULL);
        free(logger_sender);
        logger_sender = NULL;
    } else {
        #ifdef DEBUG
            printf("Logger: Ready\n");
        #endif
    }

    return logger_sender;
}

/**
 * @brief  Initialization of message queue for logger
 */
int logger_init_mq()
{
    #ifdef DEBUG
        printf("Logger: Initializing message queue..\n");
    #endif

    int mq_id = msgget(IPC_PRIVATE, 0666);

    return mq_id;
}

/**
 * @brief  Get log message type string from enum
 * @param  type_buf: Buffer where type string will be put 
 * @param  len: Buffer len, should be equal to MIN_TYPE_STR_SIZE
 * @param  type: log messge type
 */
void get_log_msg_type(char* type_buf, int len, log_msg_type_t type) {
    if (len < MIN_TYPE_STR_SIZE) {  // assert?
        errno = EINVAL;
        warn_on_error("get_log_msg_type");
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
        default:
             sprintf(type_buf, "NONE");
    }
}

/**
 * @brief  Get current time for log
 * @param  date_buf: Buffer where time will be put 
 * @param  len: Buffer len, should be equal to TIME_STR_SIZE
 */
void get_log_time(char* time_buf, int len)
{
    if (len != TIME_STR_SIZE) {  // assert?
        errno = EINVAL;
        warn_on_error("get_log_time");
    }

    time_t curr_time = time(NULL);
    struct tm *tm = localtime(&curr_time);

    strftime(time_buf, len, "%H:%M:%S", tm);
}

/**
 * @brief  Get current date for log
 * @param  date_buf: Buffer where date will be put 
 * @param  len: Buffer len, should be equal to DATE_STR_SIZE
 */
void get_log_date(char* date_buf, int len)
{
    if (len != DATE_STR_SIZE) {  // assert?
        errno = EINVAL;
        warn_on_error("get_log_date");
    }

    time_t curr_time = time(NULL);
    struct tm *tm = localtime(&curr_time);

    strftime(date_buf, len, "%Y_%m_%d", tm);
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
    get_log_date(date_buf, DATE_STR_SIZE);

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
    char time_buf[TIME_STR_SIZE];
    get_log_time(time_buf, TIME_STR_SIZE);

    char type_buf[MIN_TYPE_STR_SIZE];
    get_log_msg_type(type_buf, MIN_TYPE_STR_SIZE, log_msg.msg_payload.type);

    int full_msg_size = LOG_TEXT_LEN + TIME_STR_SIZE + MIN_TYPE_STR_SIZE;
    char msg[full_msg_size];
    snprintf(msg, full_msg_size, "%s | [%s] : %s", time_buf, type_buf, log_msg.msg_payload.text);

    if (logger.options & FILE_PRINT) {
        fprintf(logger.file, "%s\n", msg);
    }
    if (logger.options & CONSOLE_PRINT) {
        printf("%s\n", msg);
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
    int log_msg_sz = sizeof(log_msg.msg_payload);

    // Tell parent process we are ready
    log_msg.msg_type = SYSTEM;
    log_msg.msg_payload.type = SYSTEM;
    snprintf(log_msg.msg_payload.text, sizeof(LOG_MSG_READY), LOG_MSG_READY);
    if (msgsnd(logger.mq_id, &log_msg, log_msg_sz, 0) < 0)
        return -1;

    #ifdef DEBUG
        printf("Logger: Main loop started\n");
    #endif

    int is_logger_running = 1;
    while (is_logger_running) {
        if (msgrcv(logger.mq_id, &log_msg, log_msg_sz, SYSTEM, MSG_EXCEPT) < 0) {
            warn_on_error("logger_main msgrcv");
            is_logger_running = 0;
        }

        if (strcmp(log_msg.msg_payload.text, LOG_MSG_CLOSE) == 0) {
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
        printf("Logger: Finalized\n");
    #endif
}