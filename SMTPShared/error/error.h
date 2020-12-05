#ifndef ERROR_H
#define ERROR_H

#include <errno.h>

void exit_on_error(const char* error_text);
void warn_on_error(const char* warning_text);

#endif
