#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <signal.h>

#include "server.h"

#include "autogen/server-opts.h"

#include "../SMTPShared/error/error.h"
#include "../SMTPShared/logger/logger.h"

#define BASE_LOG_DIR "."
#define BASE_MAIL_DIR "."

static int pipe_fd[2];  // Pipe file descriptors for graceful exit

/**
 * @brief Signals handler (SIGINT and SIGTERM) for graceful exit
 */
void int_handler(int dummy) {
    assert(dummy == SIGINT || dummy == SIGTERM);

    if (write(pipe_fd[1], "c", 1) < 0)
        warn_on_error("write");
}

int main(int argc, char **argv) {
    #ifdef DEBUG
        printf("Starting program..\n");
    #endif

    optionProcess(&serverOptions, argc, argv);

    if (pipe2(pipe_fd, O_NONBLOCK) == -1)
        exit_on_error("pipe2");

    signal(SIGINT, int_handler);
    signal(SIGTERM, int_handler);

    logger_t* logger;
    if (HAVE_OPT(LOG_DIR))
        logger = logger_init(OPT_ARG(LOG_DIR), CONSOLE_PRINT);
    else
        logger = logger_init(BASE_LOG_DIR, CONSOLE_PRINT);

    if (!logger) {
        close(pipe_fd[0]);
        close(pipe_fd[1]);

        exit_on_error("logger_init");
    }

    server_t* server;
    if (HAVE_OPT(MAIL_DIR))
        server = server_init(logger, OPT_VALUE_PORT, OPT_ARG(MAIL_DIR), pipe_fd[0]);
    else
        server = server_init(logger, OPT_VALUE_PORT, BASE_MAIL_DIR, pipe_fd[0]);

    if (server) {
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