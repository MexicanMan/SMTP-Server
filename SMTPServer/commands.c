#include "commands.h"

const char* smtp_commands[SMTP_CMDS] = {
    HELO_CMD, EHLO_CMD, MAIL_CMD, RCPT_CMD, DATA_CMD, RSET_CMD, QUIT_CMD, VRFY_CMD
};

