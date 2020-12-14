#include <string.h>

#include "commands.h"

int helo_handle(char* msg, int msg_len, server_t* server, int client_ind);
int ehlo_handle(char* msg, int msg_len, server_t* server, int client_ind);
int mail_handle(char* msg, int msg_len, server_t* server, int client_ind);
int rcpt_handle(char* msg, int msg_len, server_t* server, int client_ind);
int data_handle(char* msg, int msg_len, server_t* server, int client_ind);
int rset_handle(char* msg, int msg_len, server_t* server, int client_ind);
int quit_handle(char* msg, int msg_len, server_t* server, int client_ind);
int vrfy_handle(char* msg, int msg_len, server_t* server, int client_ind);

const char* smtp_commands[SMTP_CMDS] = {
    HELO_CMD, EHLO_CMD, MAIL_CMD, RCPT_CMD, DATA_CMD, RSET_CMD, QUIT_CMD, VRFY_CMD
};

int (*command_handlers[SMTP_CMDS])(char*, int, server_t*, int) = {
    helo_handle, ehlo_handle, mail_handle, rcpt_handle, data_handle, rset_handle, quit_handle, vrfy_handle
};

/**
 * @brief Parses and handles command in message
 * @return Number of parsed command (1 or 0) or error (< 0)
 */
int commands_parse(char* msg, int msg_len, server_t* server, int client_ind) {
    int flag = 0;
    char* cmd = NULL;

    for (int i = 0; i < SMTP_CMDS && !flag; i++) {
        cmd = strstr(msg, smtp_commands[i]);
        if (cmd) {
            if (cmd != msg)
                return flag;

            int cmd_size = strlen(smtp_commands[i]) + 1;
            msg += cmd_size;
            msg_len -= cmd_size;

            if (command_handlers[i](msg, msg_len, server, client_ind) < 0)
                flag = -1;
            else
                flag = 1;
        }
    }

    return flag;
}

int helo_handle(char* msg, int msg_len, server_t* server, int client_ind) {


    return 0;
}

int ehlo_handle(char* msg, int msg_len, server_t* server, int client_ind) {
    

    return 0;    
}

int mail_handle(char* msg, int msg_len, server_t* server, int client_ind) {
    

    return 0;    
}

int rcpt_handle(char* msg, int msg_len, server_t* server, int client_ind) {
    

    return 0;    
}

int data_handle(char* msg, int msg_len, server_t* server, int client_ind) {
    

    return 0;    
}

int rset_handle(char* msg, int msg_len, server_t* server, int client_ind) {
    

    return 0;    
}

int quit_handle(char* msg, int msg_len, server_t* server, int client_ind) {
    int client_d = server->fds[client_ind].fd;
    server_client_t* client = get_item(server->clients, client_d);

    // Ignore arguments if there is some
    int new_state = server_fsm_step(client->client_state, SERVER_FSM_EV_CMD_QUIT, client_ind, server);
    if (new_state == SERVER_FSM_ST_INVALID) {
        logger_log(server->logger, ERROR_LOG, "quit_handle server_fsm_step");
        return -1;
    }

    return 0;    
}

int vrfy_handle(char* msg, int msg_len, server_t* server, int client_ind) {
    

    return 0;    
}