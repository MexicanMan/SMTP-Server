#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
 
#include "client.h"
#include "../SMTPShared/shared_strings.h"

#define BUFSIZE 400

#define END_S "\n.\n"


int batch_files_for_processes(mail_files_t* mails, int processes_count, logger_t* logger, int pipeDescr, int is_home_mode)
{
	logger_log(logger, INFO_LOG, "Started batching of mails\n");

	int start = 0;
	int end = 0;
	int mails_count = mails->count;
	int pack = mails_count/processes_count;
	if(mails_count % processes_count)
		pack++;

	while(end < mails_count - 1)
	{
		start = end;
		if(end + pack >= mails_count)
		{
			end = mails_count - 1;
		}
		else
		{
			end += pack;
		}
		//fork туть
		if(process_mail_files(mails, start, end, logger, pipeDescr, is_home_mode) != 0)
		{
			logger_log(logger, ERROR_LOG, "Error while processing mail pack\n");
			//printf("Error while processing mail pack from %d to %d\n", start, end);
			return -1;
		}
	}
	logger_log(logger, INFO_LOG, "Finished batch of mails\n");
	return 0;
}

int process_mail_files(mail_files_t* mail_files, int start_ind, int end_ind, logger_t* logger, int pipeDescr, int is_home_mode)
{
	logger_log(logger, INFO_LOG, "Processing mails pack\n");
	//Сначала - считать файлы, удалить их из директории и, затем передать их обработчику
	mail_t** mails = malloc(sizeof(mail_t) * end_ind-start_ind);
	int mail_count = 0;
	for(int i = start_ind; i <= end_ind; i++)
	{
		int str_count;
		char** mail_text = read_file(mail_files->files[i], &str_count);
		if(mail_text == NULL)
		{
			logger_log(logger, ERROR_LOG, "Error while reading mail from file\n");
			for(int j = 0; j < mail_count; j++)
			{
				clear_mail(mails[j]);
			}
			free(mails);
			return -1;
		}

		mail_t* mail = parse_mail(mail_text, str_count, is_home_mode);
		if(mail == NULL)
		{
			logger_log(logger, ERROR_LOG, "Error while parsing mail file text\n");
			for(int j = 0; j < mail_count; j++)
			{
				clear_mail(mails[j]);
			}
			clear_mail_text(mail_text);
			free(mails);
			return -1;
		}
		//Освободить считанный текст письма
		clear_mail_text(mail_text);
		mails[mail_count] = mail;
		mail_count++;
	}
	logger_log(logger, INFO_LOG, "Mails pack parsed\n");
	//Затем по идее врубить селект, создать соединения, обработать их и закрыть
	if(process_mails(mails, mail_count, logger, pipeDescr) != 0)
	{
		for(int i = 0; i < mail_count; i++)
		{
			clear_mail(mails[i]);
		}
		free(mails);
		return -1;
	}

	for(int i = 0; i < mail_count; i++)
	{
		clear_mail(mails[i]);
	}
	free(mails);
	
    return 0;
}

int process_mails(mail_t** mails, int mail_count, logger_t* logger, int pipeDescr)
{
	logger_log(logger, INFO_LOG, "Processing mails\n");
	//Инициализация структур соединений
	int conn_cnt = count_mails_connections(mails, mail_count);
	conn_t** connections = init_connections(mails, mail_count, conn_cnt, logger);
	if(connections == NULL)
	{
		logger_log(logger, ERROR_LOG, "Can't init connections\n");
		return -1;
	}

	logger_log(logger, INFO_LOG, "Connecting...\n");
	//Вызовы коннектов
	if(connections_start(connections, conn_cnt, logger) != 0)
	{
		logger_log(logger, ERROR_LOG, "Can't start connections\n");
		clear_connections(connections, conn_cnt);
		return -1;
	}
	logger_log(logger, INFO_LOG, "Connected\n");

	//Инициализация структур select-а
	fd_set saveReadFS, saveWriteFS, readFS, writeFS;
    struct timeval selectTimeout;

	FD_ZERO(&saveReadFS);
    FD_ZERO(&saveWriteFS);
    FD_ZERO(&readFS);
    FD_ZERO(&writeFS);

    FD_SET(pipeDescr, &saveReadFS);
	for(int i = 0; i < conn_cnt; i++)
	{
		FD_SET(connections[i]->socket, &saveReadFS);
	}

	readFS = saveReadFS;
	writeFS = saveWriteFS;

	int sel_run = 1;
	//Селект
	while (sel_run)
    {
		if (select(FD_SETSIZE, &readFS, &writeFS, NULL, NULL) < 0) 
		{
			logger_log(logger, INFO_LOG, "Error in select\n");
			clear_connections(connections, conn_cnt);
			return -1;
		}

		//Обработка селекта
		for (int i = 0; i < FD_SETSIZE; i++) 
		{
			if (!FD_ISSET(i, &readFS) && !FD_ISSET(i, &writeFS))
			{
				continue;
			}
				
			if (i == pipeDescr) 
			{
				logger_log(logger, INFO_LOG, "Exiting select on close\n");
				clear_connections(connections, conn_cnt);
				return -1;
			}

			conn_t* curr_conn = get_active_connection(connections, conn_cnt, i, logger);
			if (!curr_conn) 
			{
				logger_log(logger, INFO_LOG, "Can't find connection with current socket\n");
				continue;
			}

			if (FD_ISSET(i, &readFS)) 
			{
				
			}

			if (FD_ISSET(i, &writeFS)) 
			{
				
			}
		}

		if(check_connections_for_finish(connections, conn_cnt) != 0)
		{
			sel_run = 0;
		}
	}

	//Очистка
	clear_connections(connections, conn_cnt);
	logger_log(logger, INFO_LOG, "Processing mails finished\n");
    return 0;
}

conn_t** init_connections(mail_t** mails, int mail_count, int conns_count, logger_t* logger)
{
	int cci = 0; //Current connection index
	conn_t** connections = malloc(sizeof(conn_t*) * conns_count);
	if(connections == NULL)
	{
		logger_log(logger, ERROR_LOG, "Cant allocate memory for connections\n");
		return -1;
	}
	for(int i = 0; i < mail_count; i++)
	{
		mail_t* curr_mail = mails[i];
		for(int j = 0; j < curr_mail->tos_count; j++)
		{
			conn_t* new_conn = init_connection(curr_mail, j);
			if(new_conn == NULL)
			{
				logger_log(logger, ERROR_LOG, "Can't init connection\n");
				for(int k = 0; k < cci; k++)
				{
					clear_connection(connections[k]);
				}
				free(connections);
			}
			connections[cci] = new_conn;
			cci++;
		}
	}
	return connections;
}

void clear_connections(conn_t** connections, int conns_count)
{
	for(int i = 0; i < conns_count; i++)
	{
		clear_connection(connections[i]);
	}
	free(connections);
}

int connections_start(conn_t** connections, int conns_count, logger_t* logger)
{
	for(int i = 0; i < conns_count; i++)
	{
		if(connection_start(connections[i]) != 0)
		{
			logger_log(logger, ERROR_LOG, "Can't start connection\n");
			return -1;
		}
	}
}

int count_mails_connections(mail_t** mails, int mail_count)
{
	int cnt = 0;
	for(int i = 0; i < mail_count; i++)
	{
		cnt += mails[i]->tos_count;
	}
	return cnt;
}

conn_t* get_active_connection(conn_t** connections, int conns_count, int socket, logger_t* logger)
{

}

int check_connections_for_finish(conn_t** connections, int conns_count)
{
	int finished = 1;
	for(int i = 0; i < conns_count; i++)
	{
		if(connections[i]->state != CLIENT_FSM_EV_INVALID &&
		   connections[i]->state != CLIENT_FSM_EV_QUIT && 
		   connections[i]->state != CLIENT_FSM_ST_ERROR)
		{
			finished = 0;
		}
	}
	return finished;
}
 
int main_old(int argc, char **argv) 
{
    struct sockaddr_in addr;    /* для адреса сервера */
    int sk;                     /* файловый дескриптор сокета */
    char buf[BUFSIZE];          /* буфер для сообщений */
	char recv_buf[BUFSIZE];
    char* inp_buf = NULL;
	char* print_buf = NULL;
    int len;
    fd_set filed_set, ready_set;
 
    FD_ZERO(&ready_set);
    FD_ZERO(&filed_set);
 
    if (argc != 2) 
    {
        printf("Usage: echoc <ip>\nEx.:   echoc 10.30.0.2\n");
        exit(0);
    }
 
    /* создаём TCP-сокет */
    if ((sk = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        perror("socket");
        exit(1);
    }
 
    FD_SET(0, &filed_set);
    FD_SET(sk, &filed_set);
 
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(8080);
 
    /* соединяемся с сервером */
    if (connect(sk, (struct sockaddr*) &addr, sizeof(addr)) < 0) 
    {
        perror("connect");
        exit(1);
    }
 
    printf("Connected to Echo server. Type /q to quit.\n");
    while (1) //добавить переменную условия
    {
        ready_set = filed_set;
        if (select(sk+1, &ready_set, NULL, NULL, NULL) == -1) 
        {
            perror("select");
            exit(1);//смэрт
        }
 
        if (FD_ISSET(0, &ready_set)) 
        {
			//printf("dal\n");
            if ((len = read(0, buf, BUFSIZE-1)) < 0) 
            {
                perror("read");
                exit(1);
            }

			if (strlen(buf) == 0)
                continue;
            if (strcmp(buf, "/q") == 0)
                break;

			buf[len] = '\0';
			
			size_t input_size = 0;
			if(inp_buf != NULL)
				input_size = strlen(inp_buf);
			size_t full_len = concat_dynamic_strings(&inp_buf, buf, input_size, len);
			//Потом переделать (или написать на основе старой) функцию объединения буферов
			if(full_len < 0)
			{
				perror("concat");
                exit(1);
			}
			
			char* end = strstr(inp_buf, "\n");
			//printf("buf - %s\n", buf);
			//printf("ibuf - %s\n", inp_buf);
			if(end != NULL)
			{
				//printf("es\n");
				int pr_len = end-inp_buf;
				int end_len = strlen("\n");
				int post_len = strlen(end) - end_len;

				char* pr_buf = (char*) calloc(pr_len+end_len, sizeof(char));
				char* post_buf = (char*) calloc(post_len, sizeof(char));

				memcpy(pr_buf, inp_buf, sizeof(char) * (pr_len + end_len));
				memcpy(post_buf, end+end_len, sizeof(char) * post_len);

				//printf("pr_buf - %s\n", pr_buf);
				//printf("post_buf - %s\n", post_buf);
				
				//printf("Otsilayu - %s\n", pr_buf);
                //смэрт 2
				if (full_send(sk, pr_buf, sizeof(char) * (pr_len + end_len), 0) < 0)
		        {
		            perror("send");
		            exit(1);
		        }

				free(pr_buf);
				free(post_buf);
				free(inp_buf);
				inp_buf = NULL;

			}

        }

        if (FD_ISSET(sk, &ready_set)) 
        {
            len = recv(sk, recv_buf, BUFSIZE, 0);
            if (len < 0) 
            {
                perror("recv");
                exit(1);
            } else if (len == 0) 
            {
                printf("Remote host has closed the connection\n");
                exit(1);
            }

			size_t print_size = 0;
			if(print_buf != NULL)
				print_size = strlen(print_buf);
			size_t full_len = concat_dynamic_strings(&print_buf, recv_buf, print_size, len);
			//Потом переделать (или написать на основе старой) функцию объединения буферов
			if(full_len < 0)
			{
				perror("concat");
                exit(1);
			}
			//Накопление и печать
			
			char* end = strstr(print_buf, END_S);
			if(end != NULL)
			{
				int pr_len = end-print_buf;
				int end_len = strlen(END_S);
				int post_len = strlen(end) - end_len;

				char* pr_buf = (char*) calloc(pr_len+end_len, sizeof(char));
				char* post_buf = (char*) calloc(post_len, sizeof(char));

				memcpy(pr_buf, print_buf, sizeof(char) * (pr_len + end_len));
				memcpy(post_buf, end+end_len, sizeof(char) * post_len);

				printf("<< %s\n", print_buf);

				free(pr_buf);
				free(post_buf);
				free(print_buf);
				print_buf = NULL;

			}   
        }
    }
 
    close(sk);
 
    return 0;
}
