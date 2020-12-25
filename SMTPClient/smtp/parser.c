#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <resolv.h>

#include "../SMTPShared/shared_strings.h"
#include "parser.h"

#define MAX_STR_LENGTH 1024
#define MAX_STR_COUNT 1024
#define MAX_TO_COUNT 16
#define MAX_RECORD_LENGTH 2048

#define HOME_HOST "arasaka.com"
#define HOME_IP "127.0.0.1"
#define MAX_HOST_ADR_LEN 256

char** read_file(char* filename, int* str_num)
{
    FILE *fp;
    
    char** filetext = init_mail_text();
    int len = MAX_STR_LENGTH;
    char str[len];
    memset(str, 0, len);
    int pos = 0;

    if ((fp = fopen(filename, "r")) == NULL)
    {
        printf("Can't open file %s\n", filename);
        return NULL;
    }

    while(fgets(str, len, fp))
    {
        if(copy_str_to_arr(str, filetext, pos) != 0)
        {
            clear_mail_text(filetext);
            return NULL;
        }
        pos++;
    }

    if(fclose(fp) != 0)
    {
        printf("Error when closing file\n");
        clear_mail_text(filetext);
        return NULL;
    }

    *str_num = pos;
    return filetext;
}

int copy_str_to_arr(char* str, char** arr, int pos)
{
    int len = strlen(str) + 1;
    char* cpstr = malloc(sizeof(char) * len);
    if(cpstr == NULL)
    {
        printf("Error when allocating string for mail text\n");
        free(cpstr);
        return -1;
    }

    if(strcpy(cpstr, str) == NULL)
    {
        printf("Error when copiyng string in mail text\n");
        free(cpstr);
        return -1;
    }

    if(arr[pos] != NULL)
    {
        printf("Trying to rewrite readed data! Potential memory leak!\n");
        free(cpstr);
        return -1;
    }

    arr[pos] = cpstr;
    return 0;
}

char** init_mail_text()
{
    char** filetext = malloc(sizeof(char*) * MAX_STR_COUNT);
    if(filetext == NULL)
    {
        printf("Error when allocating array for mail text\n");
        return NULL;
    }

    for(int i = 0; i < MAX_STR_COUNT; i++)
    {
        filetext[i] = NULL;
    }
    return filetext;
}

int clear_mail_text(char** arr)
{
    if(arr == NULL)
    {
        return 0;
    }
    for(int i = 0; i < MAX_STR_COUNT; i++)
    {
        if(arr[i] == NULL)
        {
            break;
        }
        free(arr[i]);
    }
    free(arr);
    return 0;
}

mail_t* parse_mail(char** mail_file_text, int str_num, int is_home_mode)
{
    if(str_num < 3)
    {
        printf("Incorrect mail format\n");
        return NULL;
    }
    char* from_raw = mail_file_text[0];
    char** to_raws[MAX_TO_COUNT];
    char** tos[MAX_TO_COUNT];
    char** hosts[MAX_TO_COUNT];
    memset(to_raws, NULL, MAX_TO_COUNT);
    memset(tos, NULL, MAX_TO_COUNT);
    int to_count;

    int i = 1;
    while(strcmp(mail_file_text[i], "\n") != 0)
    {
        to_raws[i-1] = mail_file_text[i];
        to_count++;
    }
    

    char* from = cut_addresses_from_mail_format(from_raw);
    if(from == NULL)
    {
        printf("Error while processing mail struct addresses\n");
        return NULL;
    }

    for(int i = 0; i < MAX_TO_COUNT; i++)
    {
        char* to_raw = to_raws[i];
        if(to_raw == NULL)
        {
            break;
        }

        char* to = cut_addresses_from_mail_format(to_raw);
        if(to == NULL)
        {
            printf("Error while processing mail struct addresses\n");
            for(int j = 0; j < i; j++)
            {
                free(tos[j]);
                if(j != i-1)
                {
                    free(hosts[j]);
                }
            }
            free(from);
            return NULL;
        }
        tos[i] = to;

        char* host = cut_host_from_reciever(tos[i]);
        if(host == NULL)
        {
            printf("Error while processing mail struct addresses\n");
            for(int j = 0; j < i; j++)
            {
                free(tos[j]);
                free(hosts[j]);
            }
            free(from);
            return NULL;
        }
        hosts[i] = host;
    }
    
    char** text = malloc(sizeof(char*) * str_num - to_count - 1);
    if(text = NULL)
    {
        printf("Error while allocating memory for mail struct text\n");
        return NULL;
    }

    //Спарсить конкретные адреса серверов получателей
    //Заполнить структуру
    //Написать ф-цию освобождения структуры

}

char** get_recievers_from_hosts(char** hosts)
{
    
}

char* get_address_from_reciever(char* reciever_host, int is_home_mode)
{
    char* result_adr = NULL;
    if(is_home_mode || strcmp(reciever_host, HOME_HOST) == 0)
    {
        result_adr = malloc(sizeof(char) * MAX_HOST_ADR_LEN);
        if(result_adr == NULL)
        {
            printf("Error while allocating memory for reciever address\n");
            return NULL;
        }
        strcpy(result_adr, HOME_IP);
    }
    else
    {
        char* mx = get_record(reciever_host, ns_t_mx);
        if(mx == NULL)
        {
            printf("Error while recieving mx record\n");
            return NULL;
        }

        char* addr = get_record(mx, ns_t_a);
        if(addr == NULL)
        {
            printf("Error while recieving ip address\n");
            free(mx);
            return NULL;
        }

        free(mx);

        result_adr = malloc(sizeof(char) * MAX_HOST_ADR_LEN);
        if(result_adr == NULL)
        {
            printf("Error while allocating memory for reciever address\n");
            free(addr);
            return NULL;
        }

        strcpy(result_adr, addr);
        free(addr);
    }

    return result_adr;
}

char* get_record(char* host, int type)
{
    ns_msg msg;
    ns_rr rr;
    u_char* record[MAX_RECORD_LENGTH];
    char* rows[MAX_RECORD_LENGTH];
    memset(rows, '\0', MAX_RECORD_LENGTH);

    int resLen = res_query(host, ns_c_any, type, record, sizeof(record));
    if(resLen < 0)
    {
        printf("Error while requesting record\n");
        return NULL;
    }

    if (ns_initparse(record, resLen, &msg) < 0) 
    {
        printf("Error while requesting record\n");
        return NULL;
    }

    resLen = ns_msg_count(msg, ns_s_an);
    if (resLen == 0) 
    {
        printf("No records founded\n");
        return NULL;
    }

    if (ns_parserr(&msg, ns_s_an, 0, &rr) < 0) 
    {
        printf("Error while requesting record\n");
        return NULL;
    }

    ns_sprintrr(&msg, &rr, NULL, NULL, rows, sizeof(rows));
    //printf("\nrows - %s\n",rows);

    char* tmp = strrchr(rows, '\t') + 1;
    if(tmp == NULL)
    {
        printf("Error while parsing record\n");
        return NULL;
    }
    char* record_res = malloc(sizeof(char) * MAX_RECORD_LENGTH);
    if(record_res == NULL)
    {
        printf("Error while allocating memory for record\n");
        return NULL;
    }

    strcpy(record_res, tmp);

    //printf("\nrecord - %s\n",record_res);
    return record_res;
}

char* cut_addresses_from_mail_format(char* addr_str_raw)
{
    char* start = strchr(addr_str_raw, '<');
    char* end = strrchr(addr_str_raw, '>');
    int len = end-start;
    char* from = malloc(sizeof(char) * len);
    if(from == NULL)
    {
        printf("Error while allocating memory for mail from string\n");
        return NULL;
    }

    if(strncpy(from, start+1, len-1) == NULL)
    {
        printf("Error while copiyng mail from string\n");
        free(from);
        return NULL;
    }
    from[len-1] = '\0';
    return from;
}

char* cut_host_from_reciever(char* reciever)
{
    char* start = strchr(reciever, '@');
    char* end = strrchr(reciever, '>');
    int len = end-start;
    char* from = malloc(sizeof(char) * len);
    if(from == NULL)
    {
        printf("Error while allocating memory for reciever host from string\n");
        return NULL;
    }

    if(strncpy(from, start+1, len-1) == NULL)
    {
        printf("Error while copiyng host from string\n");
        free(from);
        return NULL;
    }
    from[len-1] = '\0';
    return from;
}