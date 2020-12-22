#include <string.h>
#include <unistd.h>

#include "fsm_handlers.h"
#include "commands_parser.h"

server_client_t* get_client_by_ind(server_t* server, int client_ind) {
    int client_d = server->fds[client_ind].fd;
    return get_item(server->clients, client_d);
}

te_server_fsm_state HANDLE_ACCEPTED(server_t* server, int client_d, te_server_fsm_state next_state) {
    logger_log(server->logger, INFO_LOG, "Adding new client");
    
    server_fill_pollfd(server, client_d, server->fd_max);

    server_client_t client = empty_client();
    client.client_state = next_state;

    if (prepare_send_buf(server->fds + server->fd_max - 1, &client, READY_RESP, sizeof(READY_RESP)) < 0) {
        logger_log(server->logger, ERROR_LOG, "HANDLE_ACCEPTED prepare_send_buf");
        return SERVER_FSM_ST_SERVER_ERROR;
    }

    if (add_item(&server->clients, client_d, client) < 0) {
        logger_log(server->logger, ERROR_LOG, "HANDLE_ACCEPTED add_item");
        return SERVER_FSM_ST_SERVER_ERROR;
    }

    logger_log(server->logger, INFO_LOG, "New client added");

    return next_state;
}

te_server_fsm_state HANDLE_HELO(server_t* server, int client_ind, te_server_fsm_state next_state) {
    logger_log(server->logger, INFO_LOG, "Client sent HELO");

    server_client_t* client = get_client_by_ind(server, client_ind);

    if (prepare_send_buf(server->fds + client_ind, client, OK_RESP, sizeof(OK_RESP)) < 0) {
        logger_log(server->logger, ERROR_LOG, "HANDLE_HELO prepare_send_buf");
        return SERVER_FSM_ST_SERVER_ERROR;
    }

    client->client_state = next_state;

    return next_state;
}

te_server_fsm_state HANDLE_EHLO(server_t* server, int client_ind, te_server_fsm_state next_state) {
    logger_log(server->logger, INFO_LOG, "Client sent EHLO");

    server_client_t* client = get_client_by_ind(server, client_ind);

    if (prepare_send_buf(server->fds + client_ind, client, OK_RESP, sizeof(OK_RESP)) < 0) {
        logger_log(server->logger, ERROR_LOG, "HANDLE_EHLO prepare_send_buf");
        return SERVER_FSM_ST_SERVER_ERROR;
    }

    client->client_state = next_state;

    return next_state;
}

te_server_fsm_state HANDLE_MAIL(server_t* server, int client_ind, const char* data, int len, te_server_fsm_state next_state) {
    logger_log(server->logger, INFO_LOG, "Client sent MAIL");

    server_client_t* client = get_client_by_ind(server, client_ind);

    // Save from
    if (client_add_from(client, data, len) < 0) {
        logger_log(server->logger, ERROR_LOG, "HANDLE_MAIL client_add_from");
        return SERVER_FSM_ST_SERVER_ERROR;
    }

    if (prepare_send_buf(server->fds + client_ind, client, OK_RESP, sizeof(OK_RESP)) < 0) {
        logger_log(server->logger, ERROR_LOG, "HANDLE_MAIL prepare_send_buf");
        return SERVER_FSM_ST_SERVER_ERROR;
    }

    client->client_state = next_state;

    return next_state;
}

te_server_fsm_state HANDLE_RCPT(server_t* server, int client_ind, const char* data, int len, te_server_fsm_state next_state) {
    logger_log(server->logger, INFO_LOG, "Client sent RCPT");
    
    const char* resp = NULL;
    int resp_len = 0;

    server_client_t* client = get_client_by_ind(server, client_ind);

    // Save to
    int addition_res = client_add_to(client, data, len, server->domain);
    if (addition_res > 0) {
        resp = OK_RESP;
        resp_len = sizeof(OK_RESP);
    } else if (addition_res == 0) {
        resp = STORAGE_RESP;
        resp_len = sizeof(STORAGE_RESP);
    } else {
        logger_log(server->logger, ERROR_LOG, "HANDLE_RCPT client_add_to");
        return SERVER_FSM_ST_SERVER_ERROR;
    }

    if (prepare_send_buf(server->fds + client_ind, client, resp, resp_len) < 0) {
        logger_log(server->logger, ERROR_LOG, "HANDLE_RCPT prepare_send_buf");
        return SERVER_FSM_ST_SERVER_ERROR;
    }

    client->client_state = next_state;

    return next_state;
}

te_server_fsm_state HANDLE_DATA(server_t* server, int client_ind, te_server_fsm_state next_state) {
    logger_log(server->logger, INFO_LOG, "Client sent DATA");

    server_client_t* client = get_client_by_ind(server, client_ind);

    if (prepare_send_buf(server->fds + client_ind, client, START_MAIL_RESP, sizeof(START_MAIL_RESP)) < 0) {
        logger_log(server->logger, ERROR_LOG, "HANDLE_DATA prepare_send_buf");
        return SERVER_FSM_ST_SERVER_ERROR;
    }

    client->client_state = next_state;

    return next_state;
}

te_server_fsm_state HANDLE_MAIL_RECEIVED(server_t* server, int client_ind, 
                                         const char* data, int len, te_server_fsm_state next_state) {
    logger_log(server->logger, INFO_LOG, "Client sent whole mail");

    server_client_t* client = get_client_by_ind(server, client_ind);

    // Save data
    if (client_add_data(client, data, len) < 0) {
        logger_log(server->logger, ERROR_LOG, "HANDLE_MAIL_RECEIVED client_add_data");
        return SERVER_FSM_ST_SERVER_ERROR;
    }

    // Save mail
    if (client_save_mail(client, server->maildir, server->client_mail_dir, server->domain) < 0) {
        logger_log(server->logger, ERROR_LOG, "HANDLE_MAIL_RECEIVED client_save_mail");
        return SERVER_FSM_ST_SERVER_ERROR;
    }
    
    // Clean mail after success save
    reset_client_mail(client);

    if (prepare_send_buf(server->fds + client_ind, client, OK_RESP, sizeof(OK_RESP)) < 0) {
        logger_log(server->logger, ERROR_LOG, "HANDLE_MAIL_RECEIVED prepare_send_buf");
        return SERVER_FSM_ST_SERVER_ERROR;
    }

    client->client_state = next_state;
    
    return next_state;
}

te_server_fsm_state HANDLE_QUIT(server_t* server, int client_ind, te_server_fsm_state next_state) {
    logger_log(server->logger, INFO_LOG, "Client wants to quit");

    server_client_t* client = get_client_by_ind(server, client_ind);

    if (prepare_send_buf(server->fds + client_ind, client, QUIT_RESP, sizeof(QUIT_RESP)) < 0) {
        logger_log(server->logger, ERROR_LOG, "HANDLE_QUIT prepare_send_buf");
        return SERVER_FSM_ST_SERVER_ERROR;
    }

    client->client_state = next_state;

    return next_state;
}

te_server_fsm_state HANDLE_VRFY(server_t* server, int client_ind, te_server_fsm_state next_state) {
    logger_log(server->logger, INFO_LOG, "Client sent VRFY");

    server_client_t* client = get_client_by_ind(server, client_ind);

    // Unimplemented command
    if (prepare_send_buf(server->fds + client_ind, client, UNIMPL_CMD_RESP, sizeof(UNIMPL_CMD_RESP)) < 0) {
        logger_log(server->logger, ERROR_LOG, "HANDLE_VRFY prepare_send_buf");
        return SERVER_FSM_ST_SERVER_ERROR;
    }

    client->client_state = next_state;

    return next_state;
}

te_server_fsm_state HANDLE_RSET(server_t* server, int client_ind, te_server_fsm_state next_state) {
    logger_log(server->logger, INFO_LOG, "Client sent RSET");

    server_client_t* client = get_client_by_ind(server, client_ind);

    reset_client_mail(client);

    if (prepare_send_buf(server->fds + client_ind, client, OK_RESP, sizeof(OK_RESP)) < 0) {
        logger_log(server->logger, ERROR_LOG, "HANDLE_RSET prepare_send_buf");
        return SERVER_FSM_ST_SERVER_ERROR;
    }

    client->client_state = next_state;

    return next_state;
}

te_server_fsm_state HANDLE_CLOSE(server_t* server, int client_ind, te_server_fsm_state next_state) {
    logger_log(server->logger, INFO_LOG, "Client leaving");

    int client_d = server->fds[client_ind].fd;

    int last_fd_ind = server->fd_max - 1;
    if (client_ind < last_fd_ind)
        memcpy(server->fds + client_ind, server->fds + client_ind + 1, sizeof(struct pollfd) * (last_fd_ind - client_ind));
    server->fd_max = last_fd_ind;

    del_item(&server->clients, client_d);

    close(client_d);

    logger_log(server->logger, INFO_LOG, "Client left");

    return next_state;
}
