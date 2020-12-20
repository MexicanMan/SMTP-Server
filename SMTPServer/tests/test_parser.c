#include "test_parser.h"

#include "../commands_parser.h"

#define KEY 0

#define HELO "HELO\r\n"
#define HELO_DOMAIN "HELO example.com\r\n"
#define EHLO "EHLO\r\n"
#define MAIL_EMPTY "MAIL FROM: <>\r\n"
#define MAIL_ADDRESS "MAIL FROM: <a@a.com>\r\n"
#define RCPT "RCPT TO: <a@a.com>\r\n"
#define DATA "DATA\r\n"
#define RSET "RSET\r\n"
#define VRFY "VRFY\r\n"
#define VRFY_DOMAIN "VRFY a\r\n"
#define QUIT "QUIT\r\n"

#define BAD_CMD "CMD"
#define BAD_HELO "HELO"
#define BAD_MAIL "MAIL FROM:\r\n"
#define BAD_RCPT "RCPT TO:\r\n"

static server_t* server_mock;

int init_parser_suite() {
    server_mock = (server_t*) malloc(sizeof(server_t));
    server_mock->fd_max = 0;
    server_mock->clients = NULL;
    server_client_t client_mock = empty_client();
    add_item(&server_mock->clients, KEY, client_mock);
    server_mock->logger = logger_init(".", 0);

    parser_initalize(NULL);

    return 0;
}

int cleanup_parser_suite() {
    parser_finalize();

    client_dict_free(&server_mock->clients);
    logger_free(server_mock->logger);

    if (server_mock)
        free(server_mock);

    return 0;
}

void test_good_commands_parse() {
    int res = commands_parse(HELO, sizeof(HELO), server_mock, KEY);
    CU_ASSERT_EQUAL(res, 1);

    res = commands_parse(HELO_DOMAIN, sizeof(HELO_DOMAIN), server_mock, KEY);
    CU_ASSERT_EQUAL(res, 1);

    res = commands_parse(EHLO, sizeof(EHLO), server_mock, KEY);
    CU_ASSERT_EQUAL(res, 1);

    res = commands_parse(MAIL_EMPTY, sizeof(MAIL_EMPTY), server_mock, KEY);
    CU_ASSERT_EQUAL(res, 1);

    res = commands_parse(MAIL_ADDRESS, sizeof(MAIL_ADDRESS), server_mock, KEY);
    CU_ASSERT_EQUAL(res, 1);

    res = commands_parse(RCPT, sizeof(RCPT), server_mock, KEY);
    CU_ASSERT_EQUAL(res, 1);

    res = commands_parse(DATA, sizeof(DATA), server_mock, KEY);
    CU_ASSERT_EQUAL(res, 1);

    res = commands_parse(RSET, sizeof(RSET), server_mock, KEY);
    CU_ASSERT_EQUAL(res, 1);

    res = commands_parse(VRFY, sizeof(VRFY), server_mock, KEY);
    CU_ASSERT_EQUAL(res, 1);

    res = commands_parse(VRFY_DOMAIN, sizeof(VRFY_DOMAIN), server_mock, KEY);
    CU_ASSERT_EQUAL(res, 1);

    res = commands_parse(QUIT, sizeof(QUIT), server_mock, KEY);
    CU_ASSERT_EQUAL(res, 1);
}

void test_bad_commands_parse() {
    int res = commands_parse(BAD_CMD, sizeof(BAD_CMD), server_mock, KEY);
    CU_ASSERT_EQUAL(res, 0);

    res = commands_parse(BAD_HELO, sizeof(BAD_HELO), server_mock, KEY);
    CU_ASSERT_EQUAL(res, 0);

    res = commands_parse(BAD_MAIL, sizeof(BAD_MAIL), server_mock, KEY);
    CU_ASSERT_EQUAL(res, 0);

    res = commands_parse(BAD_RCPT, sizeof(BAD_RCPT), server_mock, KEY);
    CU_ASSERT_EQUAL(res, 0);
}

int fill_parser_suite(CU_pSuite suite) {
    if (!CU_add_test(suite, "Commands parse good test", test_good_commands_parse) ||
        !CU_add_test(suite, "Commands parse bad test", test_bad_commands_parse)) {
        return CU_get_error();
    }

    return CUE_SUCCESS;
}