#include "test_parser.h"

#include "../smtp/parser.h"

#define TEST_MAIL_FN "./SMTPClient/test_mails/mail_1"

static char** mailtext;
static int num;
static mail_t* test_mail;

int init_parser_tests()
{
    return 0;
}

void test_read_file()
{
    int str_num;
    mailtext = read_file(TEST_MAIL_FN, &str_num); 
    num = str_num;
    CU_ASSERT_PTR_NOT_NULL(mailtext);
    CU_ASSERT_PTR_NOT_NULL(mailtext[0]);
    CU_ASSERT_EQUAL(str_num, 7);
    CU_ASSERT_STRING_EQUAL(mailtext[0], "X-FROM: <V@arasaka.com>\n");
}

void test_parse_mail()
{
    char* from = cut_addresses_from_mail_format(mailtext[0]);
    test_mail = parse_mail(mailtext, num, 0);
    CU_ASSERT_STRING_EQUAL(from, "V@arasaka.com");
    CU_ASSERT_PTR_NOT_NULL(test_mail);
    CU_ASSERT_PTR_NOT_NULL(test_mail->from);
    CU_ASSERT_STRING_EQUAL(test_mail->from, "V@arasaka.com");
    CU_ASSERT_PTR_NOT_NULL(test_mail->recievers);
    CU_ASSERT_PTR_NOT_NULL(test_mail->hosts);
    CU_ASSERT_EQUAL(test_mail->tos_count, 2);
    CU_ASSERT_EQUAL(test_mail->text_len, 3);
    CU_ASSERT_STRING_EQUAL(test_mail->mail_text[2], "Let's f*ck Arasaka!");
    free(from);
}

void test_parse_addr()
{
    char* addr = get_address_from_reciever("yandex.ru", 0);
    char* home_addr = get_address_from_reciever("any.com", 1);
    CU_ASSERT_STRING_EQUAL(home_addr, "127.0.0.1");
    free(addr);
    free(home_addr);
}

int test_parser(CU_pSuite suite)
{
    if (!CU_add_test(suite, "File read test", test_read_file) ||
        !CU_add_test(suite, "Mail text parse test", test_parse_mail) ||
        !CU_add_test(suite, "Address parse test", test_parse_addr)) 
    {
        return CU_get_error();
    }

    return CUE_SUCCESS;
}

int cleanup_parser_tests()
{
    if(mailtext != NULL)
        clear_mail_text(mailtext);
    clear_mail(test_mail);
    return 0;
}