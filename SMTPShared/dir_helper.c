#include <sys/stat.h>

#include "dir_helper.h"

int create_dir_if_not_exists(const char* path) {
    struct stat st;
    if (stat(path, &st) == -1)
        if (mkdir(path, 0700) == -1)
            return -1;
    
    return 0;
}