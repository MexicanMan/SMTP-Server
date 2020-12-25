#ifndef TEST_PARCER_H
#define TEST_PARCER_H

#include <CUnit/Basic.h>

int init_parcer_tests();
int cleanup_parcer_tests();
void test_read_file();
int test_parcer(CU_pSuite suite);

#endif