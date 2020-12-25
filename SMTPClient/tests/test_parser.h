#ifndef TEST_PARCER_H
#define TEST_PARCER_H

#include <CUnit/Basic.h>

int init_parser_tests();
int cleanup_parser_tests();
void test_read_file();
void test_parse_mail();
int test_parser(CU_pSuite suite);

#endif