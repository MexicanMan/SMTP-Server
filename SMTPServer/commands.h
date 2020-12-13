#ifndef COMMANDS_H
#define COMMANDS_H

#include "autogen/server-fsm.h"

#define EOL "\r\n"
#define EOM "\r\n.\r\n"

#define SMTP_CMDS 8

#define HELO_CMD "HELO"
#define EHLO_CMD "EHLO"
#define MAIL_CMD "MAIL FROM:"
#define RCPT_CMD "RCPT TO:"
#define DATA_CMD "DATA"
#define RSET_CMD "RSET"
#define QUIT_CMD "QUIT"
#define VRFY_CMD "VRFY"

#define READY_RESP "220 Service ready\r\n"
#define CLOSE_RESP "221 Service closing transmission channel\r\n"

#endif
