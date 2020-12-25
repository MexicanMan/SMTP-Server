#include "test_parcer.h"

#include "../smtp/parcer.h"

#define TEST_MAIL_FN "./SMTPClient/test_mails/mail_1"

char** mailtext;

int init_parcer_tests()
{
    return 0;
}

void test_read_file()
{
    mailtext = read_file(TEST_MAIL_FN); 

    CU_ASSERT_PTR_NOT_NULL(mailtext);
    CU_ASSERT_PTR_NOT_NULL(mailtext[0]);
    CU_ASSERT_STRING_EQUAL(mailtext[0], "X-FROM: <test@tmp.ru>\n");
}

int test_parcer(CU_pSuite suite)
{
    if (!CU_add_test(suite, "File read test", test_read_file)) 
    {
        return CU_get_error();
    }

    return CUE_SUCCESS;
}

int cleanup_parcer_tests()
{
    if(mailtext != NULL)
        clear_mail_text(mailtext);
    return 0;
}