#ifndef TEST_MAIL_H
#define TEST_MAIL_H

#include <CUnit/Basic.h>

int init_mail_suite();
int cleanup_mail_suite();
int fill_mail_suite(CU_pSuite suite);

#endif