#include <CUnit/Basic.h>

#include "test_mail.h"

int main() {
    int ret;

    printf("Starting tests..\n");

    if (CU_initialize_registry() != CUE_SUCCESS) {
        perror("CU_initialize_registry");
        return CU_get_error();
    }

    CU_pSuite mail_suite = CU_add_suite("Mail module tests", init_mail_suite, cleanup_mail_suite);
    if (!mail_suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if ((ret = fill_mail_suite(mail_suite)) != CUE_SUCCESS) {
        CU_cleanup_registry();
        return ret;
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    CU_cleanup_registry();

    printf("Tests completed");
    return CU_get_error();
}