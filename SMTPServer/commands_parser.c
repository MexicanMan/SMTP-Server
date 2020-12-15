#include <string.h>
#include <stdlib.h>
#include <pcre.h>  

#include "commands_parser.h"

#define SUB_STR_VEC_LEN 10

int helo_handle(const char* msg, int msg_len, server_t* server, int client_ind);
int ehlo_handle(const char* msg, int msg_len, server_t* server, int client_ind);
int mail_handle(const char* msg, int msg_len, server_t* server, int client_ind);
int rcpt_handle(const char* msg, int msg_len, server_t* server, int client_ind);
int data_handle(const char* msg, int msg_len, server_t* server, int client_ind);
int rset_handle(const char* msg, int msg_len, server_t* server, int client_ind);
int quit_handle(const char* msg, int msg_len, server_t* server, int client_ind);
int vrfy_handle(const char* msg, int msg_len, server_t* server, int client_ind);

struct pcre_compiled {
    pcre* complied_regexp;
    pcre_extra* extra_regexp;
};

const char* smtp_commands_regexp[SMTP_CMDS] = {
    HELO_CMD_REGEXP, EHLO_CMD_REGEXP, MAIL_CMD_REGEXP, RCPT_CMD_REGEXP, 
    DATA_CMD_REGEXP, RSET_CMD_REGEXP, QUIT_CMD_REGEXP, VRFY_CMD_REGEXP
};

struct pcre_compiled smtp_compiled_regexp[SMTP_CMDS];

int (*command_handlers[SMTP_CMDS])(const char*, int, server_t*, int) = {
    helo_handle, ehlo_handle, mail_handle, rcpt_handle, data_handle, rset_handle, quit_handle, vrfy_handle
};

/**
 * @brief Initialize parser with prepared regexps
 */
int parser_initalize(logger_t* logger) {
    const char *pcre_error;
    int pcre_error_offset;

    for (int i = 0; i < SMTP_CMDS; i++) {
        pcre* regexp_compiled = pcre_compile(
            smtp_commands_regexp[i], PCRE_ANCHORED, &pcre_error, &pcre_error_offset, NULL);
        if(regexp_compiled == NULL) {
            logger_log(logger, ERROR_LOG, "parser_initalize pcre_compile");
            return -1;
        }

        pcre_extra* pcre_extra = pcre_study(regexp_compiled, 0, &pcre_error);
        if(pcre_error != NULL) {
            logger_log(logger, ERROR_LOG, "parser_initalize pcre_study");
            return -1;
        }

        smtp_compiled_regexp[i].complied_regexp = regexp_compiled;
        smtp_compiled_regexp[i].extra_regexp = pcre_extra;
    }

    return 0;
}

/**
 * @brief  Parses and handles command in a message
 * @return Number of parsed command (1 or 0) or error (< 0)
 */
int commands_parse(char* msg, int msg_len, server_t* server, int client_ind) {
    int flag = 0;
    int sub_str_vec[SUB_STR_VEC_LEN];

    for (int i = 0; i < SMTP_CMDS && !flag; i++) {
        int pcre_ret = pcre_exec(smtp_compiled_regexp[i].complied_regexp, 
            smtp_compiled_regexp[i].extra_regexp, msg, msg_len, 0, 0, sub_str_vec, SUB_STR_VEC_LEN);

        if (pcre_ret == PCRE_ERROR_NOMEMORY || pcre_ret == PCRE_ERROR_UNKNOWN_NODE) {
            flag = -1; 
        } else if (pcre_ret > 0) {
            const char *args = NULL;
            int len = 0;
            if (pcre_ret > 1) {
                pcre_get_substring(msg, sub_str_vec, pcre_ret, 1, &(args));
                len = sub_str_vec[3] - sub_str_vec[2];
            }

            if (command_handlers[i](args, len, server, client_ind) < 0)
                flag = -1;
            else
                flag = 1;

            if (args) 
                pcre_free_substring(args);
        }
    }

    return flag;
}

/**
 * @brief Finalize and clear parser
 */
void parser_finalize() {
    for (int i = 0; i < SMTP_CMDS; i++) {
        pcre_free(smtp_compiled_regexp[i].complied_regexp);
        if(smtp_compiled_regexp[i].extra_regexp != NULL)
            pcre_free_study(smtp_compiled_regexp[i].extra_regexp);
    }
}

int helo_handle(const char* msg, int msg_len, server_t* server, int client_ind) {
    int client_d = server->fds[client_ind].fd;
    server_client_t* client = get_item(server->clients, client_d);

    // Ignore domain as long as it isn't used for now
    int new_state = server_fsm_step(client->client_state, SERVER_FSM_EV_CMD_HELO, client_ind, server, NULL, 0);
    if (new_state == SERVER_FSM_ST_INVALID) {
        if (prepare_send_buf(server->fds + client_ind, client, BAD_SEQ_RESP, sizeof(BAD_SEQ_RESP)) < 0) {
            logger_log(server->logger, ERROR_LOG, "helo_handle prepare_send_buf");
            return -1;
        }
    } else if (new_state == SERVER_FSM_ST_SERVER_ERROR) {
        logger_log(server->logger, ERROR_LOG, "helo_handle server_fsm_step");
        return -1;
    }

    return 0;
}

int ehlo_handle(const char* msg, int msg_len, server_t* server, int client_ind) {
    int client_d = server->fds[client_ind].fd;
    server_client_t* client = get_item(server->clients, client_d);

    // Currently similar to helo
    // Ignore domain as long as it isn't used for now
    int new_state = server_fsm_step(client->client_state, SERVER_FSM_EV_CMD_EHLO, client_ind, server, NULL, 0);
    if (new_state == SERVER_FSM_ST_INVALID) {
        if (prepare_send_buf(server->fds + client_ind, client, BAD_SEQ_RESP, sizeof(BAD_SEQ_RESP)) < 0) {
            logger_log(server->logger, ERROR_LOG, "ehlo_handle prepare_send_buf");
            return -1;
        }
    } else if (new_state == SERVER_FSM_ST_SERVER_ERROR) {
        logger_log(server->logger, ERROR_LOG, "ehlo_handle server_fsm_step");
        return -1;
    }

    return 0;   
}

int mail_handle(const char* msg, int msg_len, server_t* server, int client_ind) {
    int client_d = server->fds[client_ind].fd;
    server_client_t* client = get_item(server->clients, client_d);

    // Pass msg as it should be address
    int new_state = server_fsm_step(client->client_state, SERVER_FSM_EV_CMD_MAIL, client_ind, server, msg, msg_len);
    if (new_state == SERVER_FSM_ST_INVALID) {
        if (prepare_send_buf(server->fds + client_ind, client, BAD_SEQ_RESP, sizeof(BAD_SEQ_RESP)) < 0) {
            logger_log(server->logger, ERROR_LOG, "mail_handle prepare_send_buf");
            return -1;
        }
    } else if (new_state == SERVER_FSM_ST_SERVER_ERROR) {
        logger_log(server->logger, ERROR_LOG, "mail_handle server_fsm_step");
        return -1;
    }

    return 0;    
}

int rcpt_handle(const char* msg, int msg_len, server_t* server, int client_ind) {
    int client_d = server->fds[client_ind].fd;
    server_client_t* client = get_item(server->clients, client_d);

    // Pass msg as it should be address
    int new_state = server_fsm_step(client->client_state, SERVER_FSM_EV_CMD_RCPT, client_ind, server, msg, msg_len);
    if (new_state == SERVER_FSM_ST_INVALID) {
        if (prepare_send_buf(server->fds + client_ind, client, BAD_SEQ_RESP, sizeof(BAD_SEQ_RESP)) < 0) {
            logger_log(server->logger, ERROR_LOG, "rcpt_handle prepare_send_buf");
            return -1;
        }
    } else if (new_state == SERVER_FSM_ST_SERVER_ERROR) {
        logger_log(server->logger, ERROR_LOG, "rcpt_handle server_fsm_step");
        return -1;
    }

    return 0;    
}

int data_handle(const char* msg, int msg_len, server_t* server, int client_ind) {
    int client_d = server->fds[client_ind].fd;
    server_client_t* client = get_item(server->clients, client_d);

    // Ignore arguments if there is some
    int new_state = server_fsm_step(client->client_state, SERVER_FSM_EV_CMD_DATA, client_ind, server, NULL, 0);
    if (new_state == SERVER_FSM_ST_SERVER_ERROR) {
        logger_log(server->logger, ERROR_LOG, "data_handle server_fsm_step");
        return -1;
    }

    return 0;
}

int rset_handle(const char* msg, int msg_len, server_t* server, int client_ind) {
    int client_d = server->fds[client_ind].fd;
    server_client_t* client = get_item(server->clients, client_d);

    // Ignore arguments if there is some
    int new_state = server_fsm_step(client->client_state, SERVER_FSM_EV_CMD_RSET, client_ind, server, NULL, 0);
    if (new_state == SERVER_FSM_ST_SERVER_ERROR) {
        logger_log(server->logger, ERROR_LOG, "rset_handle server_fsm_step");
        return -1;
    }

    return 0;    
}

int quit_handle(const char* msg, int msg_len, server_t* server, int client_ind) {
    int client_d = server->fds[client_ind].fd;
    server_client_t* client = get_item(server->clients, client_d);

    // Ignore arguments if there is some
    int new_state = server_fsm_step(client->client_state, SERVER_FSM_EV_CMD_QUIT, client_ind, server, NULL, 0);
    if (new_state == SERVER_FSM_ST_SERVER_ERROR) {
        logger_log(server->logger, ERROR_LOG, "quit_handle server_fsm_step");
        return -1;
    }

    return 0;    
}

int vrfy_handle(const char* msg, int msg_len, server_t* server, int client_ind) {
    int client_d = server->fds[client_ind].fd;
    server_client_t* client = get_item(server->clients, client_d);

    // Ignore arguments as long as they aren't used for now
    int new_state = server_fsm_step(client->client_state, SERVER_FSM_EV_CMD_VRFY, client_ind, server, NULL, 0);
    if (new_state == SERVER_FSM_ST_SERVER_ERROR) {
        logger_log(server->logger, ERROR_LOG, "vrfy_handle server_fsm_step");
        return -1;
    }

    return 0;   
}