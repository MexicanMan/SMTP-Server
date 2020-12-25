#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../SMTPShared/shared_strings.h"
#include "parcer.h"

#define MAX_STR_LENGTH 1024
#define MAX_STR_COUNT 1024

char** read_file(char* filename)
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

mail_t* parce_mail(char* mail_file_text)
{

}

int fill_recievers_in_mail(mail_t* mail)
{

}

recs_t* get_address_from_reciever(char* reciever_adr)
{

}
