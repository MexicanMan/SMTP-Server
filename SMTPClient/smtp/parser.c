#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <resolv.h>

#include "../SMTPShared/shared_strings.h"
#include "parser.h"

#define MAX_STR_LENGTH 255
#define MAX_STR_COUNT 1000000
#define MAX_TO_COUNT 16
#define MAX_RECORD_LENGTH 2048

#define HOME_HOST "arasaka.com"
#define HOME_IP "127.0.0.1"
#define HOME_PORT 2525
#define SMTP_PORT 25
#define MAX_HOST_ADR_LEN 256

#define END_OF_STR "\r\n"

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

    memset(filetext, 0, MAX_STR_COUNT);

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
    char* to_raws[MAX_TO_COUNT];
    char** tos;
    char* hosts[MAX_TO_COUNT];
    char** addrs;
    memset(to_raws, 0, MAX_TO_COUNT);
    memset(hosts, 0, MAX_TO_COUNT);
    int to_count = 0;
    int rhc = 0;

    int sn = 1;
    while(strcmp(mail_file_text[sn], "\n") != 0 && strcmp(mail_file_text[sn], "\r\n") != 0)
    {
        to_raws[sn-1] = mail_file_text[sn];
        to_count++;
        sn++;
    }

    mail_t* mail = malloc(sizeof(mail_t));
    if(mail == NULL)
    {
        printf("Error while allocating mail struct\n");
        return NULL;
    }

    tos = malloc(sizeof(char*) * MAX_TO_COUNT);
    if(tos == NULL)
    {
        printf("Error while allocating recievers array\n");
        free(mail);
        return NULL;
    }
    memset(tos, 0, MAX_TO_COUNT);

    char* from = cut_addresses_from_mail_format(from_raw);
    if(from == NULL)
    {
        printf("Error while processing mail struct addresses\n");
        free(mail);
        free(tos);
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
            free(mail);
            free(tos);
            return NULL;
        }
        

        char* host = cut_host_from_reciever(to);
        if(host == NULL)
        {
            printf("Error while processing mail struct addresses\n");
            for(int j = 0; j < i; j++)
            {
                if(j != i-1)
                {
                    free(tos[j]);
                }
                free(hosts[j]);
            }
            free(to);
            free(from);
            free(mail);
            free(tos);
            return NULL;
        }
        else if(strcmp(host, HOME_HOST) == 0 && is_home_mode == 0)
        {
            free(host);
            free(to);
        }
        else
        {
            hosts[rhc] = host;
            tos[rhc] = to;
            rhc++;
        }
        
    }

    int* ports = malloc(sizeof(int) * MAX_TO_COUNT);
    if(ports == NULL)
    {
        printf("Error while allocating ports memory\n");
        for(int i = 0; i < rhc; i++)
        {
            free(tos[i]);
            free(hosts[i]);
        }
        free(from);
        free(mail);
        free(tos);
        return NULL;
    }

    addrs = get_recievers_from_hosts(hosts, ports, is_home_mode);
    if(addrs == NULL)
    {
        printf("Error while getting recievers addresses\n");
        for(int i = 0; i < to_count; i++)
        {
            free(tos[i]);
            free(hosts[i]);
        }
        free(ports);
        free(from);
        free(mail);
        free(tos);
        return NULL;
    }

    char** text = malloc(sizeof(char*) * str_num - to_count - 1);
    if(text == NULL)
    {
        printf("Error while allocating memory for mail struct text\n");
        for(int i = 0; i < rhc; i++)
        {
            free(tos[i]);
            free(hosts[i]);
            free(addrs[i]);
        }
        free(ports);
        free(from);
        free(addrs);
        free(mail);
        free(tos);
        return NULL;
    }

    for(int i = 0; i < str_num - to_count - 2; i++)
    {
        text[i] = malloc(sizeof(char) * MAX_STR_LENGTH);
        if(text[i] == 0)
        {
            printf("Error while allocating memory for mail struct text\n");
            for(int j = 0; j < i; j++)
            {
                free(text[j]);
            }
            for(int j = 0; j < to_count; j++)
            {
                free(tos[j]);
                free(hosts[j]);
                free(addrs[j]);
            }
            free(ports);
            free(from);
            free(addrs);
            free(text);
            free(mail);
            free(tos);
            return NULL;
        }
        strcpy(text[i], mail_file_text[i + to_count + 2]);
    }
    
    for(int i = 0; i < to_count; i++)
    {
        free(hosts[i]);
    }

    mail->from = from;
    mail->recievers = tos;
    mail->hosts = addrs;
    mail->ports = ports;
    mail->mail_text = text;
    mail->tos_count = rhc;
    mail->text_len = str_num - to_count - 2;
    return mail;
}

void clear_mail(mail_t* mail)
{
    if(mail == NULL)
    {
        return;
    }

    for(int i = 0; i < mail->tos_count; i++)
    {   
        free(mail->recievers[i]);
        free(mail->hosts[i]);
    }
    free(mail->hosts);
    free(mail->recievers);
    free(mail->ports);
    for(int i = 0; i < mail->text_len; i++)
    {
        free(mail->mail_text[i]);
    }
    free(mail->mail_text);
    free(mail->from);
    free(mail);
    return;
}

char** get_recievers_from_hosts(char** hosts, int* ports, int is_home_mode)
{
    int i = 0;
    char** addrs = malloc(sizeof(char*) * MAX_TO_COUNT);
    if(addrs == NULL)
    {
        printf("Error while allocating memory for mail struct text\n");
        return NULL;
    }
    memset(addrs, 0, MAX_TO_COUNT);

    int rhc = 0;
    while(hosts[i] != NULL && i < MAX_TO_COUNT)
    {
        char* addr = get_address_from_reciever(hosts[i], ports+rhc, is_home_mode);
        if(addr == NULL)
        {
            printf("Error while recieving recievers addresses\n");
            for(int j = 0; j < rhc; j++)
            {
                free(addrs[j]);
            }
            free(addrs);
            return NULL;
        }
        else if(strcmp(addr, "home") == 0)
        {
            printf("Reciever - home server. Ignoring.\n");
        }
        else
        {
            addrs[rhc] = addr;
            rhc++;
        }
        i++;
    }
    return addrs;
}

char* get_address_from_reciever(char* reciever_host, int* port, int is_home_mode)
{
    char* result_adr = "home";
    if(strcmp(reciever_host, HOME_HOST) == 0)
    {
        if(is_home_mode)
        {
            result_adr = malloc(sizeof(char) * MAX_HOST_ADR_LEN);
            if(result_adr == NULL)
            {
                printf("Error while allocating memory for reciever address\n");
                return NULL;
            }
            strcpy(result_adr, HOME_IP);
            *port = HOME_PORT;
        }
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
        *port = SMTP_PORT;
        free(addr);
    }

    return result_adr;
}

char* get_record(char* host, int type)
{
    ns_msg msg;
    ns_rr rr;
    u_char record[MAX_RECORD_LENGTH];
    char rows[MAX_RECORD_LENGTH];
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

    char* tmp_sp = strrchr(rows, ' ') + 1;
    char* tmp_tb = strrchr(rows, '\t') + 1;
    char* tmp;
    if(tmp_sp > tmp_tb)
    {
        tmp = tmp_sp;
    }
    else
    {
        tmp = tmp_tb;
    }
    
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
    char* end = strrchr(reciever, '\0');
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

char* try_parse_message_part(char** buf, int bufsize, int* len, int* new_len)
{
    //printf("parsing - %s", *buf);
    char* find_eos = strstr(*buf, END_OF_STR);
    if(find_eos == NULL)
    {
        *len = 0;
        return NULL;
    }
    else
    {
        *len = find_eos - *buf + strlen(END_OF_STR);
        char* msg = malloc(sizeof(char) * *len);
        if(msg == NULL)
        {
            //printf("Error while allocating memory for message part\n");
            *len = -1;
            return NULL;
        }
        //printf("memcpy: msg - %s, buf - %s, len - %d\n", msg, *buf, *len);
        memcpy(msg, *buf, *len);

        int new_size = bufsize - *len;
        /*
        char* new_start = *buf+*len+strlen(END_OF_STR);
        char* new_buf = malloc(sizeof(char) * new_size);
        if(new_buf == NULL)
        {
            printf("Error while allocating memory for message\n");
            *len = -1;
            free(msg);
            return NULL;
        }
        */
        //strcpy(new_buf, new_start);
        //free(*buf);
        //printf("\n debug b: new_len - %d, *new_len - %d, new_size - %d\n", new_len, *new_len, new_size);
        *new_len = new_size;
        //printf("\t debug a: new_len - %d, *new_len - %d, new_size - %d", new_len, *new_len, new_size);
        //*buf = new_buf;
        //printf("memcpy: buf - %s, len - %d, new_size - %d\n", *buf, *len, new_size);
        memcpy(*buf, *buf + *len, sizeof(char) * new_size);

        return msg;
    }
    
}

int parse_return_code(char* buf)
{
    char num[3];
    strncpy(num, buf, 3);
    int res = atoi(num);
    return res;
}