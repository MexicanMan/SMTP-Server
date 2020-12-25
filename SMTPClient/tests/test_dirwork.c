#include "test_dirwork.h"

#include "../dirwork/dir_worker.h"

#define TEST_MAIL_DIR "./SMTPClient/test_mails"
static int files_count = 0;
static mail_files_t* files;


int init_dirwork_tests()
{
    return 0;
}

void test_read_files_count()
{
    files_count = get_files_count(TEST_MAIL_DIR);
    CU_ASSERT_EQUAL(files_count, 3);
}

void test_read_files()
{
    files = check_directory(TEST_MAIL_DIR);

    CU_ASSERT_PTR_NOT_NULL(files);
    CU_ASSERT_EQUAL(files->count, 3);
    CU_ASSERT_PTR_NOT_NULL(files->files);
}

int test_dirwork(CU_pSuite suite)
{
    if (!CU_add_test(suite, "Files count test", test_read_files_count) ||
        !CU_add_test(suite, "Filenames read test", test_read_files)) 
    {
        return CU_get_error();
    }

    return CUE_SUCCESS;
}

int cleanup_dirwork_tests()
{
    if(files != NULL)
        clear_mail_files(files);
    return 0;
}