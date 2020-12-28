#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "../SMTPShared/logger/logger.h"
#include "./client.h"
#include "./dirwork/dir_worker.h"

#define BASE_LOG_DIR "./SMTPClient/build/log"
#define MAILDIR "./SMTPClient/work_mails"
#define PROC_COUNT 5
#define CHECK_PAUSE 1
#define IS_HOME_MODE 1


static volatile int run = 1;
static volatile logger_t* logger;
static int pipeDescrs[2] = { 0, 0 };

int main_old(int argc, char **argv);
static void close_handler(int signal);
int main_loop();

int main(int argc, char **argv) 
{
	int ret;

	if (pipe(pipeDescrs) < 0) 
	{
        printf("Can't init pipe\n");
        return -1;
    }

	signal(SIGINT, close_handler);
    signal(SIGTERM, close_handler);

	logger = logger_init(BASE_LOG_DIR, CONSOLE_PRINT | FILE_PRINT);
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

int main_loop()
{
	while(run)
	{
		//Проверка директории, чтение, отправка, сон, завершение
		mail_files_t* mails = check_directory(MAILDIR);
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
			
			if(batch_files_for_processes(mails, PROC_COUNT, logger, pipeDescrs[0], IS_HOME_MODE) != 0)
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