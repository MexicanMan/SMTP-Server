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


int batch_files_for_processes(mail_files_t* mails, int processes_count, logger_t* logger)
{
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
		//fork with save of pid and other sht
		//Мб и сначала врубить селект, а всю дичь делать уже потом
		//do smth
		if(process_mail_files(mails, start, end) != 0)
		{
			logger_log(logger, ERROR_LOG, "Error while processing mail pack\n");
			//printf("Error while processing mail pack from %d to %d\n", start, end);
			return -1;
		}
	}
	return 0;
}

int process_mail_files(mail_files_t* mails, int start_ind, int end_ind)
{
	//Сначала - считать файлы, удалить их из директории и, затем передать их обработчику
	//затем по идее врубить селект, создать соединения, обработать их и закрыть
    return 0;
}

int process_mails(char* mail_text/*заменить на структурированные письма*/)
{
    return 0;
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
