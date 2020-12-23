#ifndef DIR_WORKER_H
#define DIR_WORKER_H

typedef struct Mail_files{
    char** files;
    int count;
} mail_files;

mail_files* check_directory(char* dir_path);
int get_files_count(char* dir_path);
int get_files_names(char* dir_path, char** files_names, int files_count);
int clear_mail_files(mail_files* files);

#endif
