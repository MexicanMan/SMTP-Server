/** 
 * @file
 * @brief Main entry point file
 */

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "../SMTPShared/logger/logger.h"
#include "../SMTPShared/dir_helper.h"
#include "./client.h"
#include "./dirwork/dir_worker.h"
#include "./autogen/client-opts.h"

#define BASE_LOG_DIR "./SMTPClient/build/log"
#define BASE_MAILDIR "./SMTPClient/work_mails"
#define BASE_PROC_COUNT 1
#define CHECK_PAUSE 1
#define BASE_IS_HOME_MODE 1


static volatile int run = 1;
static logger_t* logger;
static int pipeDescrs[2] = { 0, 0 };

static void close_handler(int signal);
int main_loop();

/**
 * @brief Client options struct
 */
typedef struct options_struct 
{
    const char* log_dir;            ///< Directory to store log files
    const char* maildir;            ///< Directory with local mails

    int home_mode;               ///< Is client in home send mode
    int proc_count;              ///< Number of working processes
} options_t;

options_t client_options;

options_t fill_options() 
{
    options_t options = { .home_mode = OPT_VALUE_HOME_MODE};
    options.proc_count = HAVE_OPT(PROC_COUNT) ? OPT_ARG(PROC_COUNT) : BASE_PROC_COUNT;
    options.maildir = HAVE_OPT(MAIL_DIR) ? OPT_ARG(MAIL_DIR) : BASE_MAILDIR;
    options.log_dir = HAVE_OPT(LOG_DIR) ? OPT_ARG(LOG_DIR) : BASE_LOG_DIR;

    return options;
}

int validate_options(options_t options) 
{
    if (create_dir_if_not_exists(options.log_dir) < 0)
        return -1;

    return 0;
}


int main(int argc, char **argv) 
{
	optionProcess(&clientOptions, argc, argv);
    client_options = fill_options();
    if (validate_options(client_options) < 0) 
	{
        exit_on_error("validate_options");
    }

	int ret;

	if (pipe(pipeDescrs) < 0) 
	{
        printf("Can't init pipe\n");
        return -1;
    }

	signal(SIGINT, close_handler);
    signal(SIGTERM, close_handler);

	logger = logger_init(client_options.log_dir, CONSOLE_PRINT | FILE_PRINT);
    if (!logger) 
	{
        printf("Can't init logger\n");
		close(pipeDescrs[0]);
    	close(pipeDescrs[1]);
		return -1;
    }


	ret = main_loop();

	logger_free(logger);
	close(pipeDescrs[0]);
	close(pipeDescrs[1]);
	return ret;
}

/**
 * @brief Main client loop func
 */
int main_loop()
{
	while(run)
	{
		//Проверка директории, чтение, отправка, сон, завершение
		mail_files_t* mails = check_directory(client_options.maildir);
		if(mails == NULL)
		{
			logger_log(logger, ERROR_LOG, "Error while checking mails directory\n");
			//printf("Error while checking mails directory\n");
			run = 0;
		}
		else if(mails->count == 0)
		{
			logger_log(logger, INFO_LOG, "Nothing to send\n");
			//printf("Nothing to send\n");
			sleep(CHECK_PAUSE);
		}
		else
		{
			logger_log(logger, INFO_LOG, "Some mails in directory\n");
			//printf("Some mails in directory:\n");
			
			if(batch_files_for_processes(mails, client_options.proc_count, logger, pipeDescrs[0], client_options.home_mode) != 0)
			{
				logger_log(logger, ERROR_LOG, "Error while processing mails\n");
				//printf("Error while processing mails\n");
				run = 0;
				clear_mail_files(mails);
				break;
			}

			//Удаление писем
			logger_log(logger, INFO_LOG, "Deleting processed mails\n");
			for(int i = 0; i < mails->count; i++)
			{
				if(remove(mails->files[i])!= 0)
				{
					logger_log(logger, ERROR_LOG, "Error while deleting mail file\n");
				}
				else
				{
					logger_log(logger, INFO_LOG, "Mail file deleted\n");
				}
			}
			sleep(CHECK_PAUSE);
		}
		clear_mail_files(mails);
	}
	return 0;
}

static void close_handler(int sig) 
{
	write(pipeDescrs[1], "END", 3);
    run = 0;
	return;
}