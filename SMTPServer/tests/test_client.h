#ifndef TEST_CLIENT_H
#define TEST_CLIENT_H

#include <CUnit/Basic.h>

int init_client_suite();
int cleanup_client_suite();
int fill_client_suite(CU_pSuite suite);

#endif