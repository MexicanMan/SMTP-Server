#ifndef COMMANDS_PARSER_H
#define COMMANDS_PARSER_H

#include "server.h"

#include "autogen/server-fsm.h"

#include "../SMTPShared/logger/logger.h"

#define EOS '\0'
#define EOL "\r\n"
#define EOM "\r\n.\r\n"

#define SMTP_CMDS 8

#define SPACES_REGEXP "\\s*"
#define DOMAIN_REGEXP "(\\s+\\S+)?"
#define ADDRESS_OR_EMPTY_REGEXP "(<\\S+@\\S+>|<>)"
#define ADDRESS_REGEXP "(<\\S+@\\S+>)"

#define HELO_CMD_REGEXP "HELO" DOMAIN_REGEXP EOL
#define EHLO_CMD_REGEXP "EHLO" DOMAIN_REGEXP EOL
#define MAIL_CMD_REGEXP "MAIL FROM:" SPACES_REGEXP ADDRESS_OR_EMPTY_REGEXP EOL
#define RCPT_CMD_REGEXP "RCPT TO:" SPACES_REGEXP ADDRESS_REGEXP EOL
#define DATA_CMD_REGEXP "DATA" EOL
#define RSET_CMD_REGEXP "RSET" EOL
#define QUIT_CMD_REGEXP "QUIT" EOL
#define VRFY_CMD_REGEXP "VRFY" DOMAIN_REGEXP EOL

#define READY_RESP "220 Service ready\r\n"
#define QUIT_RESP "221 Service closing transmission channel\r\n"
#define OK_RESP "250 Requested mail action okay, completed\r\n"

#define STORAGE_RESP "452 Requested action not taken: insufficient system storage\r\n"

#define BAD_CMD_RESP "500 Syntax error, command unrecognised\r\n"
#define BAD_ARGS_RESP "501 Syntax error in parameters or arguments\r\n"
#define UNIMPL_CMD_RESP "502 Command not implemented\r\n"
#define BAD_SEQ_RESP "503 Bad sequence of commands\r\n"

int parser_initalize(logger_t* logger);
int commands_parse(char* msg, int msg_len, server_t* server, int client_ind);
void parser_finalize();

#endif
