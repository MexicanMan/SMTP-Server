#ifndef COMMANDS_H
#define COMMANDS_H

#include "server.h"

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
#define QUIT_RESP "221 Service closing transmission channel\r\n"
#define OK_RESP "250 Requested mail action okay, completed\r\n"

#define BAD_CMD_RESP "500 Syntax error, command unrecognised\r\n"
#define UNIMPL_CMD_RESP "502 Command not implemented\r\n"
#define BAD_SEQ_RESP "503 Bad sequence of commands\r\n"

int commands_parse(char* msg, int msg_len, server_t* server, int client_ind);

#endif
