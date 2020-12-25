#include <CUnit/Basic.h>

#include "test_dirwork.h"
#include "test_parser.h"

int main() {
    int ret;

    printf("Started client tests\n");

    if (CU_initialize_registry() != CUE_SUCCESS) {
        perror("CU_initialize_registry");
        return CU_get_error();
    }

    CU_pSuite dirwork = CU_add_suite("Directory read tests", init_dirwork_tests, cleanup_dirwork_tests);
    if (!dirwork) 
    {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if ((ret = test_dirwork(dirwork)) != CUE_SUCCESS) 
    {
        CU_cleanup_registry();
        return ret;
    }

    CU_pSuite parse = CU_add_suite("File parse tests", init_parser_tests, cleanup_parser_tests);
    if (!parse) 
    {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if ((ret = test_parser(parse)) != CUE_SUCCESS) 
    {
        CU_cleanup_registry();
        return ret;
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    //CU_cleanup_registry();

    printf("Client tests passed\n");
    return CU_get_error();
}