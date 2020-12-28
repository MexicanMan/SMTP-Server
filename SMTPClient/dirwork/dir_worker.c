#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>

#include "dir_worker.h"

mail_files_t* check_directory(char* dir_path)
{
    char** files_names = NULL;
    struct Mail_files* result = malloc(sizeof(mail_files_t));
    if(result == NULL)
    {
        printf("Error while allocating memory for mail files struct\n");
        return NULL;
    }

	int count = get_files_count(dir_path);
    if(count < 0)
    {
        printf("Error in counting files in directory %s\n", dir_path);
        free(result);
        return NULL;
    }
    else if(count == 0)
    {
        //printf("There is no files in working directory %s\n", dir_path);
        result->count = 0;
        result->files = files_names;
        return result;
    }

    files_names = (char**)malloc(sizeof(char*) * count);
    if(files_names == NULL)
    {
        printf("Error while allocating memory for files names\n");
        free(result);
        return NULL;
    }

    for(int i = 0; i < count; i++)
    {
        files_names[i] = (char*) malloc(sizeof(char) * PATH_MAX);
        if(files_names[i] == NULL)
        {
            printf("Error while allocating memory for file name\n");

            for(int j = i-1; j >= 0; j--)
            {
                free(files_names[j]);
            }
            free(files_names);
            free(result);

            return NULL;
        }
    }
    if(get_files_names(dir_path, files_names, count) != 0)
    {
        printf("Error in getting files names\n");
        for(int i = 0; i < count; i++)
        {
            free(files_names[i]);
        }
        free(files_names);
        free(result);
        return NULL;
    }
    result->count = count;
    result->files = files_names;
    return result; 
}

int get_files_count(char* dir_path)
{
    DIR* dir = NULL;
    struct dirent* entry = NULL;
    int res = 0;
    unsigned count = 0;
    
    dir = opendir(dir_path);
    if( dir == NULL ) 
    {
        printf("Error in opening %s\n", dir_path);
        return -1;
    }
    entry = readdir(dir);
    while (entry != NULL)
    {
        if(entry->d_type == DT_REG)
        {
            count++;
        }
        entry = readdir(dir);
    }

    res = closedir(dir);
    if(res != 0)
    {
        printf("Error while closing directory %s\n", dir_path);
        return -1;
    }

    return count;
}

int get_files_names(char* dir_path, char** files_names, int files_count)
{
    DIR* dir = NULL;
    struct dirent* entry = NULL;
    int res = 0;
    unsigned count = 0;
    
    dir = opendir(dir_path);
    if( dir == NULL ) 
    {
        printf("Error in opening %s\n", dir_path);
        return -1;
    }

    while ((entry = readdir(dir)) && (count < files_count))
    {
        if(entry->d_type == DT_REG)
        {
            char* name = files_names[count];
            name = strcpy(name, dir_path);
            name = strcat(name, "/");
            name = strcat(name, entry->d_name);
            count++;
        }
    }

    res = closedir (dir);
    if(res != 0)
    {
        printf("Error while closing directory %s\n", dir_path);
        return -1;
    }

    return 0;
}

int clear_mail_files(mail_files_t* files)
{
    int count = files->count;
    char** files_names = files->files;
    for(int i = 0; i < count; i++)
    {
        free(files_names[i]);
    }
    free(files_names);
    free(files);
    return 0;
}
