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
 
#include "../SMTPShared/full_io.h"

#define BUFSIZE 2048
 
int main(int argc, char **argv) 
{
    struct sockaddr_in addr;    /* для адреса сервера */
    socklen_t addrlen;          /* размер структуры с адресом */
    int sk;                     /* файловый дескриптор сокета */
    char buf[BUFSIZE];          /* буфер для сообщений */
    char inp_buf[BUFSIZE];
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
    while (1) 
    {
        ready_set = filed_set;
        if (select(sk+1, &ready_set, NULL, NULL, NULL) == -1) 
        {
            perror("select");
            exit(1);
        }
 
        if (FD_ISSET(0, &ready_set)) {
            if ((len = read(0, buf, BUFSIZE-2)) < 0) {
                perror("read");
                exit(1);
            }

            if (len < BUFSIZE-2) {
                buf[len-1] = EOS;
                buf[len] = EOT;
                len = len + 1;
            }
            else
            {
                buf[len] = EOS;
                buf[len+1] = EOT;
                len = BUFSIZE;
            }
            
            if (strlen(buf) == 0)
                continue;
            if (strcmp(buf, "/q") == 0)
                break;

            if (send(sk, buf, len, 0) < 0) {
                perror("send");
                exit(1);
            }
        }

        if (FD_ISSET(sk, &ready_set)) {
            len = recv(sk, buf, BUFSIZE, 0);
            if (len < 0) {
                perror("recv");
                exit(1);
            } else if (len == 0) {
                printf("Remote host has closed the connection\n");
                exit(1);
            }

            buf[len] = '\0';
            printf("<< %s\n", buf);
        }
    }
 
    close(sk);
 
    return 0;
}
