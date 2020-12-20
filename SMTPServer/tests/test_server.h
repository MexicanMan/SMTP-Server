#ifndef TEST_SERVER_H
#define TEST_SERVER_H

#include <CUnit/Basic.h>

int init_server_suite();
int cleanup_server_suite();
int fill_server_suite(CU_pSuite suite);

#endif