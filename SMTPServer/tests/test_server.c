#include "test_server.h"

#include "../server.h"

#define TEST_TEXT "TEST"

static server_t server_mock;

int init_server_suite() {
    return 0;
}

int cleanup_server_suite() {
    return 0;
}

void test_server_fill_pollfd() {
    int key = 1, ind = 0;

    server_fill_pollfd(&server_mock, key, ind);
    CU_ASSERT_EQUAL(server_mock.fd_max, 1);

    CU_ASSERT_EQUAL(server_mock.fds[ind].fd, key);
    CU_ASSERT_EQUAL(server_mock.fds[ind].events, POLLIN);
    CU_ASSERT_EQUAL(server_mock.fds[ind].revents, 0);

    CU_ASSERT_EQUAL(server_mock.fds_timeouts[ind], 0);
}

void test_prepare_send_buf() {
    server_client_t client = empty_client();

    int res = prepare_send_buf(server_mock.fds, &client, TEST_TEXT, sizeof(TEST_TEXT), 0);
    CU_ASSERT_EQUAL(res, 0);

    CU_ASSERT_EQUAL(server_mock.fds[0].events, POLLIN | POLLOUT);

    CU_ASSERT_EQUAL(client.out_len, sizeof(TEST_TEXT));
    CU_ASSERT_NSTRING_EQUAL(client.out_buf, TEST_TEXT, sizeof(TEST_TEXT));
}

int fill_server_suite(CU_pSuite suite) {
    if (!CU_add_test(suite, "Fill pollfd test", test_server_fill_pollfd) ||
        !CU_add_test(suite, "Parse send buf test", test_prepare_send_buf)) {
        return CU_get_error();
    }

    return CUE_SUCCESS;
}