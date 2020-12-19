#include <stdlib.h>

#include "test_mail.h"

#include "../mail.h"

int init_mail_suite() {
    return 0;
}

int cleanup_mail_suite() {
    return 0;
}

void test_empty_mail() {
    mail_t mail = empty_mail();

    CU_ASSERT_PTR_NULL(mail.from);
    for (int i = 0; i < MAX_TO; i++)
        CU_ASSERT_PTR_NULL(mail.to[i]);
    CU_ASSERT_EQUAL(mail.to_len, 0);
    CU_ASSERT_EQUAL(mail.to_type, MAIL_NONE);
    CU_ASSERT_PTR_NULL(mail.data);
}

int fill_mail_suite(CU_pSuite suite) {
    if (!CU_add_test(suite, "Empty mail test", test_empty_mail)) {
        return CU_get_error();
    }

    return CUE_SUCCESS;
}