#include "test_fsm_handlers.h"

#include "../fsm_handlers.h"

#define TEST_STATE SERVER_FSM_ST_READY
#define TEST_TEXT "TEST"
#define KEY 0

static server_t* server_mock;

int init_fsm_handlers_suite() {
    server_mock = (server_t*) malloc(sizeof(server_t));
    server_mock->fd_max = 0;
    server_mock->clients = NULL;
    server_mock->logger = logger_init(".", 0);

    return 0;
}

int cleanup_fsm_handlers_suite() {
    client_dict_free(&server_mock->clients);
    logger_free(server_mock->logger);

    if (server_mock)
        free(server_mock);

    return 0;
}

void test_HANDLE_ACCEPTED() {
    te_server_fsm_state state = HANDLE_ACCEPTED(server_mock, KEY, TEST_STATE);
    CU_ASSERT_EQUAL(state, TEST_STATE);

    server_client_dict_t* clients = server_mock->clients;
    CU_ASSERT_PTR_NOT_NULL_FATAL(clients);
    CU_ASSERT_EQUAL(clients->key, KEY);

    server_client_t client = clients->value;
    CU_ASSERT_EQUAL(client.client_state, TEST_STATE);
}

void test_HANDLE_HELO() {
    te_server_fsm_state state = HANDLE_HELO(server_mock, KEY, TEST_STATE);
    CU_ASSERT_EQUAL(state, TEST_STATE);

    server_client_t client = server_mock->clients->value;
    CU_ASSERT_EQUAL(client.client_state, TEST_STATE);
}

void test_HANDLE_EHLO() {
    te_server_fsm_state state = HANDLE_EHLO(server_mock, KEY, TEST_STATE);
    CU_ASSERT_EQUAL(state, TEST_STATE);

    server_client_t client = server_mock->clients->value;
    CU_ASSERT_EQUAL(client.client_state, TEST_STATE);
}

void test_HANDLE_MAIL() {
    te_server_fsm_state state = HANDLE_MAIL(server_mock, KEY, TEST_TEXT, sizeof(TEST_TEXT), TEST_STATE);
    CU_ASSERT_EQUAL(state, TEST_STATE);

    server_client_t client = server_mock->clients->value;
    CU_ASSERT_EQUAL(client.client_state, TEST_STATE);
    
    mail_t mail = client.mail;
    CU_ASSERT_NSTRING_EQUAL(mail.from, TEST_TEXT, sizeof(TEST_TEXT));
}

void test_HANDLE_RCPT() {
    te_server_fsm_state state = HANDLE_RCPT(server_mock, KEY, TEST_TEXT, sizeof(TEST_TEXT), TEST_STATE);
    CU_ASSERT_EQUAL(state, TEST_STATE);

    server_client_t client = server_mock->clients->value;
    CU_ASSERT_EQUAL(client.client_state, TEST_STATE);

    mail_t mail = client.mail;
    CU_ASSERT_EQUAL(mail.to_len, 1);
    CU_ASSERT_NSTRING_EQUAL(mail.to[0], TEST_TEXT, sizeof(TEST_TEXT));
}

void test_HANDLE_DATA() {
    te_server_fsm_state state = HANDLE_DATA(server_mock, KEY, TEST_STATE);
    CU_ASSERT_EQUAL(state, TEST_STATE);

    server_client_t client = server_mock->clients->value;
    CU_ASSERT_EQUAL(client.client_state, TEST_STATE);
}

void test_HANDLE_QUIT() {
    te_server_fsm_state state = HANDLE_QUIT(server_mock, KEY, TEST_STATE);
    CU_ASSERT_EQUAL(state, TEST_STATE);

    server_client_t client = server_mock->clients->value;
    CU_ASSERT_EQUAL(client.client_state, TEST_STATE);
}

void test_HANDLE_VRFY() {
    te_server_fsm_state state = HANDLE_VRFY(server_mock, KEY, TEST_STATE);
    CU_ASSERT_EQUAL(state, TEST_STATE);

    server_client_t client = server_mock->clients->value;
    CU_ASSERT_EQUAL(client.client_state, TEST_STATE);
}

void test_HANDLE_RSET() {
    te_server_fsm_state state = HANDLE_RSET(server_mock, KEY, TEST_STATE);
    CU_ASSERT_EQUAL(state, TEST_STATE);

    server_client_t client = server_mock->clients->value;
    CU_ASSERT_EQUAL(client.client_state, TEST_STATE);

    mail_t mail = client.mail;
    CU_ASSERT_PTR_NULL(mail.from);
    CU_ASSERT_EQUAL(mail.to_type, MAIL_NONE);
    CU_ASSERT_EQUAL(mail.to_len, 0);
    CU_ASSERT_PTR_NULL(mail.to[0]);
    CU_ASSERT_PTR_NULL(mail.data);
}

int fill_fsm_handlers_suite(CU_pSuite suite) {
    if (!CU_add_test(suite, "HANDLE ACCEPTED test", test_HANDLE_ACCEPTED) ||
        !CU_add_test(suite, "HANDLE HELO test", test_HANDLE_HELO) ||
        !CU_add_test(suite, "HANDLE EHLO test", test_HANDLE_EHLO) ||
        !CU_add_test(suite, "HANDLE MAIL test", test_HANDLE_MAIL) ||
        !CU_add_test(suite, "HANDLE RCPT test", test_HANDLE_RCPT) ||
        !CU_add_test(suite, "HANDLE DATA test", test_HANDLE_DATA) ||
        !CU_add_test(suite, "HANDLE QUIT test", test_HANDLE_QUIT) ||
        !CU_add_test(suite, "HANDLE VEFY test", test_HANDLE_VRFY) ||
        !CU_add_test(suite, "HANDLE RSET test", test_HANDLE_RSET)) {
        return CU_get_error();
    }

    return CUE_SUCCESS;
}