#ifndef TEST_DIRWORK_H
#define TEST_DIRWORK_H

#include <CUnit/Basic.h>

int init_dirwork_tests();
int cleanup_dirwork_tests();
int test_read_files_count();
int test_read_files();
int test_dirwork(CU_pSuite suite);
//Мб ещё на общую функцию и освобождение памяти?

#endif