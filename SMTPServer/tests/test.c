#include <CUnit/Basic.h>

#include "test_mail.h"
#include "test_client.h"
#include "test_parser.h"
#include "test_fsm_handlers.h"
#include "test_server.h"

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

    CU_pSuite client_suite = CU_add_suite("Client module tests", init_client_suite, cleanup_client_suite);
    if (!client_suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if ((ret = fill_client_suite(client_suite)) != CUE_SUCCESS) {
        CU_cleanup_registry();
        return ret;
    }

    CU_pSuite parser_suite = CU_add_suite("Parser module tests", init_parser_suite, cleanup_parser_suite);
    if (!parser_suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if ((ret = fill_parser_suite(parser_suite)) != CUE_SUCCESS) {
        CU_cleanup_registry();
        return ret;
    }

    CU_pSuite fsm_handlers_suite = CU_add_suite("FSM handlers module tests", 
        init_fsm_handlers_suite, cleanup_fsm_handlers_suite);
    if (!fsm_handlers_suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if ((ret = fill_fsm_handlers_suite(fsm_handlers_suite)) != CUE_SUCCESS) {
        CU_cleanup_registry();
        return ret;
    }

    CU_pSuite server_suite = CU_add_suite("Server module tests", init_server_suite, cleanup_server_suite);
    if (!server_suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if ((ret = fill_server_suite(server_suite)) != CUE_SUCCESS) {
        CU_cleanup_registry();
        return ret;
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    CU_cleanup_registry();

    printf("Tests completed");
    return CU_get_error();
}