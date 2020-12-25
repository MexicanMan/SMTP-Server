#include "test_parser.h"

#include "../smtp/parser.h"

#define TEST_MAIL_FN "./SMTPClient/test_mails/mail_1"

char** mailtext;

int init_parser_tests()
{
    return 0;
}

void test_read_file()
{
    int* str_num;
    mailtext = read_file(TEST_MAIL_FN, str_num); 

    CU_ASSERT_PTR_NOT_NULL(mailtext);
    CU_ASSERT_PTR_NOT_NULL(mailtext[0]);
    CU_ASSERT_EQUAL(*str_num, 6);
    CU_ASSERT_STRING_EQUAL(mailtext[0], "X-FROM: <test@tmp.ru>\n");
}

void test_parse_mail()
{
    char* from = cut_addresses_from_mail_format(mailtext[0]);
    CU_ASSERT_STRING_EQUAL(from, "test@tmp.ru");
}

int test_parser(CU_pSuite suite)
{
    if (!CU_add_test(suite, "File read test", test_read_file) ||
        !CU_add_test(suite, "Mail text parse test", test_parse_mail)) 
    {
        return CU_get_error();
    }

    return CUE_SUCCESS;
}

int cleanup_parser_tests()
{
    if(mailtext != NULL)
        clear_mail_text(mailtext);
    return 0;
}