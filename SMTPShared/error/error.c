#include <stdio.h>
#include <stdlib.h>

#include "error.h"

void exit_on_error(const char* error_text) {
    perror(error_text);
    
    exit(errno);
}

void warn_on_error(const char* warning_text) {
    perror(warning_text);
}