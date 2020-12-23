#include "test_client.h"

#include "../client.h"

#define TEST_STR "TEST"

static server_client_dict_t *clients_mock;

int init_client_suite() {
    clients_mock = NULL;

    return 0;
}

int cleanup_client_suite() {
    client_dict_free(&clients_mock);
    
    return 0;
}

void test_empty_client() {
    server_client_t client = empty_client();

    CU_ASSERT_PTR_NULL(client.inp_buf);
    CU_ASSERT_PTR_NULL(client.out_buf);
    CU_ASSERT_EQUAL(client.inp_len, 0);
    CU_ASSERT_EQUAL(client.out_len, 0);
}

void test_client_add_from() {
    server_client_t client = empty_client();
    client_add_from(&client, TEST_STR, sizeof(TEST_STR));

    CU_ASSERT_NSTRING_EQUAL(client.mail.from, TEST_STR, sizeof(TEST_STR));
}

void test_client_add_to() {
    server_client_t client = empty_client();
    client_add_to(&client, TEST_STR, sizeof(TEST_STR), TEST_STR);

    CU_ASSERT_EQUAL_FATAL(client.mail.to_len, 1);
    CU_ASSERT_EQUAL(client.mail.to_type, LOCAL_MAIL);
    CU_ASSERT_NSTRING_EQUAL(client.mail.to[client.mail.to_len-1], TEST_STR, sizeof(TEST_STR));
}

void test_client_add_data() {
    server_client_t client = empty_client();
    client_add_data(&client, TEST_STR, sizeof(TEST_STR));

    CU_ASSERT_NSTRING_EQUAL(client.mail.data, TEST_STR, sizeof(TEST_STR));
}

void test_add_item() {
    int key = 1;
    int res = add_client_by_key(&clients_mock, key, empty_client());

    CU_ASSERT_EQUAL_FATAL(res, 0);
    CU_ASSERT_PTR_NOT_NULL(clients_mock);
    CU_ASSERT_EQUAL(clients_mock->key, key);
}

void test_get_item() {
    server_client_t* res = get_client_by_key(clients_mock, 1);

    CU_ASSERT_PTR_NOT_NULL_FATAL(res);
}

void test_del_item() {
    int res = del_client_by_key(&clients_mock, 1);

    CU_ASSERT_EQUAL_FATAL(res, 0);
    CU_ASSERT_PTR_NULL_FATAL(clients_mock);
}

int fill_client_suite(CU_pSuite suite) {
    if (!CU_add_test(suite, "Empty client test", test_empty_client) ||
        !CU_add_test(suite, "Client add from test", test_client_add_from) ||
        !CU_add_test(suite, "Client add to test", test_client_add_to) ||
        !CU_add_test(suite, "Client add data test", test_client_add_data) ||
        !CU_add_test(suite, "Client add item test", test_add_item) ||
        !CU_add_test(suite, "Client get item test", test_get_item) ||
        !CU_add_test(suite, "Client del item test", test_del_item)) {
        return CU_get_error();
    }

    return CUE_SUCCESS;
}