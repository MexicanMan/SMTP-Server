/** 
 * @file
 * @brief Main entry point file
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <signal.h>
#include <grp.h>
#include <pwd.h>

#include "server.h"

#include "autogen/server-opts.h"

#include "../SMTPShared/error/error.h"
#include "../SMTPShared/logger/logger.h"
#include "../SMTPShared/dir_helper.h"

#define BASE_LOG_DIR "./log"
#define BASE_MAIL_DIR "./mail"
#define BASE_CLIENT_MAIL_DIR "./client_mail"

#define SERVER_DOMAIN "@arasaka.com"

/**
 * @brief Server options struct
 */
typedef struct options_struct {
    const char* address;            ///< Server address
    int port;                       ///< Server port

    const char* log_dir;            ///< Directory to store log files
    const char* maildir;            ///< Directory with local mails
    const char* client_mail_dir;    ///< Directory with mail to client

    const char* user;               ///< User name for privilege drop
    const char* group;              ///< Group name for privilege drop
} options_t;

static int pipe_fd[2];  ///< Pipe file descriptors for graceful exit

/**
 * @brief Signals handler (SIGINT and SIGTERM) for graceful exit
 */
void int_handler(int dummy) {
    assert(dummy == SIGINT || dummy == SIGTERM);

    if (write(pipe_fd[1], "c", 1) < 0)
        warn_on_error("write");
}

options_t fill_options() {
    options_t options = { .address = OPT_ARG(ADDRESS), .port = OPT_VALUE_PORT };
    options.log_dir = HAVE_OPT(LOG_DIR) ? OPT_ARG(LOG_DIR) : BASE_LOG_DIR;
    options.maildir = HAVE_OPT(MAIL_DIR) ? OPT_ARG(MAIL_DIR) : BASE_MAIL_DIR;
    options.client_mail_dir = HAVE_OPT(CLIENT_MAIL_DIR) ? OPT_ARG(CLIENT_MAIL_DIR) : BASE_CLIENT_MAIL_DIR; 
    options.user = HAVE_OPT(USER) ? OPT_ARG(USER) : NULL;
    options.group = HAVE_OPT(GROUP) ? OPT_ARG(GROUP) : NULL;

    return options;
}

int validate_options(options_t options) {
    if (create_dir_if_not_exists(options.log_dir) < 0)
        return -1;
    if (create_dir_if_not_exists(options.maildir) < 0)
        return -1;
    if (create_dir_if_not_exists(options.client_mail_dir) < 0)
        return -1;
    
    if ((!options.user && options.group) || (options.user && !options.group))
        return -1;

    return 0;
}

int drop_privilege(const char* user, const char* group) {
    if (getuid() == 0 && user != NULL && group != NULL) {
        // Process is running as root, drop privileges
        struct passwd *p = getpwnam(user);
        if (!p)
            return -1;
        struct group *g = getgrnam(group);
        if (!g)
            return -1;

        if (setgid(p->pw_uid) != 0)
            return -1;
        if (setuid(g->gr_gid) != 0)
            return -1;
    }

    return 0;
}

/**
 * @brief Main entry point
 */
int main(int argc, char **argv) {
    #ifdef DEBUG
        printf("Starting program..\n");
    #endif

    optionProcess(&serverOptions, argc, argv);
    options_t options = fill_options();
    if (validate_options(options) < 0) {
        errno = EINVAL;
        exit_on_error("validate_options");
    }

    if (pipe2(pipe_fd, O_NONBLOCK) == -1)
        exit_on_error("pipe2");

    signal(SIGINT, int_handler);
    signal(SIGTERM, int_handler);

    logger_t* logger = logger_init(options.log_dir, CONSOLE_PRINT | FILE_PRINT);
    if (!logger) {
        close(pipe_fd[0]);
        close(pipe_fd[1]);

        exit_on_error("logger_init");
    }
    
    server_t* server = server_init(logger, options.address, options.port, SERVER_DOMAIN, 
        options.maildir, options.client_mail_dir, pipe_fd[0]);
    if (server) {
        if(!drop_privilege(options.user, options.group)) 
            server_main(server);
        
        server_finalize(server);
    }   

    close(pipe_fd[0]);
    close(pipe_fd[1]);

    logger_free(logger);

    #ifdef DEBUG
        printf("Program stopped\n");
    #endif

    return 0;
}