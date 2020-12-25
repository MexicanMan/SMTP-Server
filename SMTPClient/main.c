#include <signal.h>

#include "../SMTPShared/logger/logger.h"
#include "./client.h"
#include "./dirwork/dir_worker.h"

#define BASE_LOG_DIR "./SMTPClient/build/log"
#define MAILDIR "./SMTPClient/test_mails"
#define PROC_COUNT 5


static volatile int run = 1;
static volatile logger_t* logger;

int main_old(int argc, char **argv);
static void close_handler(int signal);
int main_loop();

int main(int argc, char **argv) 
{
	int ret;
	signal(SIGINT, close_handler);
    signal(SIGTERM, close_handler);

	logger = logger_init(BASE_LOG_DIR, CONSOLE_PRINT | FILE_PRINT);
    if (!logger) 
	{
        printf("Can't init logger\n");
		return -1;
    }


	ret = main_loop();

	logger_free(logger);
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
			sleep(1);
		}
		else
		{
			logger_log(logger, INFO_LOG, "Some mails in directory:\n");
			//printf("Some mails in directory:\n");
			for(int i = 0; i < mails->count; i++)
			{
				printf("\tpath - %s\n", mails->files[i]);
			}
			
			if(batch_files_for_processes(mails, PROC_COUNT, logger) != 0)
			{
				logger_log(logger, ERROR_LOG, "Error while processing mails\n");
				//printf("Error while processing mails\n");
				run = 0;
				clear_mail_files(mails);
			}

			sleep(1);
		}
		clear_mail_files(mails);
	}
	return 0;
}

static void close_handler(int sig) 
{
    run = 0;
	return;
}