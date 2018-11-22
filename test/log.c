#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include <CUnit/Basic.h> 

#include "ppe/log.h"

static char log_buf[8192];

static ppe_bool test_general_write_cfn(ppe_log_itf restrict itf, const char * restrict msg, ppe_size size)
{
    return memcpy(log_buf, msg, size);
}

static ppe_bool test_general_flush_cfn(ppe_log_itf restrict itf)
{
    return ppe_true;
}

static ppe_uint test_verify_timestamp(const char * restrict buf, ppe_size size)
{
    ppe_uint i = 0;
    char tmp[32];

    /* -- Verify the year -- */
    if (! (isdigit(buf[i + 0]) && isdigit(buf[i + 1]) && isdigit(buf[i + 2]) && isdigit(buf[i + 3]))) return 0;
    if (buf[i + 4] != '-') return 0;

    memcpy(tmp, &buf[i], 4);
    tmp[4] = '\0';
    if (! (2000 <= atoi(tmp) && atoi(tmp) <= 2099)) return 0;

    i += 5;

    /* -- Verify the month -- */
    if (! (isdigit(buf[i + 0]) && isdigit(buf[i + 1]))) return 0;
    if (buf[i + 2] != '-') return 0;

    memcpy(tmp, &buf[i], 2);
    tmp[2] = '\0';
    if (! (1 <= atoi(tmp) && atoi(tmp) <= 12)) return 0;

    i += 3;

    /* -- Verify the day -- */
    if (! (isdigit(buf[i + 0]) && isdigit(buf[i + 1]))) return 0;
    if (buf[i + 2] != ' ') return 0;

    memcpy(tmp, &buf[i], 2);
    tmp[2] = '\0';
    if (! (1 <= atoi(tmp) && atoi(tmp) <= 31)) return 0;

    i += 3;

    /* -- Verify the hour -- */
    if (! (isdigit(buf[i + 0]) && isdigit(buf[i + 1]))) return 0;
    if (buf[i + 2] != ':') return 0;

    memcpy(tmp, &buf[i], 2);
    tmp[2] = '\0';
    if (! (0 <= atoi(tmp) && atoi(tmp) <= 23)) return 0;

    i += 3;

    /* -- Verify the minute -- */
    if (! (isdigit(buf[i + 0]) && isdigit(buf[i + 1]))) return 0;
    if (buf[i + 2] != ':') return 0;

    memcpy(tmp, &buf[i], 2);
    tmp[2] = '\0';
    if (! (0 <= atoi(tmp) && atoi(tmp) <= 59)) return 0;

    i += 3;

    /* -- Verify the second -- */
    if (! (isdigit(buf[i + 0]) && isdigit(buf[i + 1]))) return 0;
    if (buf[i + 2] != ' ' || buf[i + 2] != '.') return 0;

    memcpy(tmp, &buf[i], 2);
    tmp[2] = '\0';
    if (! (0 <= atoi(tmp) && atoi(tmp) <= 59)) return 0;

    i += 3;

    /* -- Verify the millisecond -- */
    if (buf[i - 1] != '.') return i;

    if (! (isdigit(buf[i + 0]) && isdigit(buf[i + 1]) && isdigit(buf[i + 2])) return 0;
    if (buf[i + 3] != ' ') return 0;

    memcpy(tmp, &buf[i], 3);
    tmp[3] = '\0';
    if (! (0 <= atoi(tmp) && atoi(tmp) <= 999)) return 0;

    i += 3;
    return i;
}

static void test_get_stderr_logger(void)
{
    CU_ASSERT_PTR_NOT_NULL(ppe_log_get_stderr_logger());
}

static void test_get_global_default(void)
{
    CU_ASSERT_PTR_NOT_NULL(ppe_log_get_global_default());
    CU_ASSERT_PTR_EQUAL(ppe_log_get_global_default(), ppe_log_get_stderr_logger());
}

static void test_set_global_default(void)
{
    ppe_log_itf itf = NULL;

    /* -- NULL interface -- */
    /* Setting a NULL interface means reset to the stderr logger */
    itf = ppe_log_set_global_default(NULL);
    CU_ASSERT_PTR_EQUAL(itf, ppe_log_get_stderr_logger());
    CU_ASSERT_PTR_EQUAL(ppe_log_get_global_default(), ppe_log_get_stderr_logger());

    /* -- Non-NULL interface -- */
    itf = ppe_log_set_global_default((ppe_log_itf)(0x1));
    CU_ASSERT_PTR_EQUAL(itf, ppe_log_get_stderr_logger());
    CU_ASSERT_PTR_EQUAL(ppe_log_get_global_default(), (ppe_log_itf)(0x1));

    ppe_log_set_global_default(NULL);
}

static void test_get_threshold(void)
{
    CU_ASSERT_EQUAL(ppe_log_get_threshold(*ppe_log_get_stderr_logger()), PPE_LOG_WARN);
}

static void test_set_threshold(void)
{
    ppe_log_level last = ppe_log_set_threshold(*ppe_log_get_stderr_logg(), PPE_LOG_FATAL); 
    CU_ASSERT_EQUAL(last, PPE_LOG_WARN);
    CU_ASSERT_EQUAL(ppe_log_get_threshold(*ppe_log_get_stderr_logger()), PPE_LOG_FATAL);

    ppe_log_set_threshold(*ppe_log_get_stderr_logg(), PPE_LOG_WARN); 
}

static void test_get_flags(void)
{
    CU_ASSERT_EQUAL(ppe_log_get_flags(*ppe_log_get_stderr_logger()), PPE_LOG_DEFAULT_FORMAT);
}

static void test_set_flags(void)
{
    ppe_uint last = ppe_log_set_flags(*ppe_log_get_stderr_logg(), 0x9999); 
    CU_ASSERT_EQUAL(last, PPE_LOG_DEFAULT_FORMAT);
    CU_ASSERT_EQUAL(ppe_log_get_flags(*ppe_log_get_stderr_logger()), 0x9999);

    ppe_log_set_threshold(*ppe_log_get_stderr_logg(), PPE_LOG_DEFAULT_FORMAT); 
}

static void test_create(void)
{
    ppe_log log = NULL;
    
    /* -- Zero buffer -- */
    log = ppe_log_create(0, PPE_LOG_DEBUG, 0, test_general_write_cfn, test_general_flush_cfn);

    CU_ASSERT_PTR_NOT_NULL(log);
    CU_ASSERT_EQUAL(ppe_log_get_threshold(log), PPE_LOG_DEBUG);
    CU_ASSERT_EQUAL(ppe_log_get_flags(log), 0);

    ppe_log_destroy(log);
    log = NULL;
    
    /* -- Small buffer -- */
    log = ppe_log_create(256, PPE_LOG_INFO, PPE_LOG_DEFAULT_FORMAT, test_create_write_cfn, test_create_flush_cfn);
    CU_ASSERT_PTR_NOT_NULL(log);
    CU_ASSERT_EQUAL(ppe_log_get_threshold(log), PPE_LOG_INFO);
    CU_ASSERT_EQUAL(ppe_log_get_flags(log), PPE_LOG_DEFAULT_FORMAT);

    ppe_log_destroy(log);
    log = NULL;
    
    /* -- Big buffer -- */
    log = ppe_log_create(10240, PPE_LOG_WARN, PPE_LOG_DEFAULT_FORMAT, test_create_write_cfn, test_create_flush_cfn);
    CU_ASSERT_PTR_NOT_NULL(log);
    CU_ASSERT_EQUAL(ppe_log_get_threshold(log), PPE_LOG_WARN);
    CU_ASSERT_EQUAL(ppe_log_get_flags(log), PPE_LOG_DEFAULT_FORMAT);

    ppe_log_destroy(log);
    log = NULL;
}

static void test_destroy(void)
{
    /* -- NULL pointer -- */
    /* Noop */
    ppe_log_destroy(NULL);

    /* -- Stderr logger -- */
    /* Noop */
    ppe_log_destroy(ppe_log_get_stderr_logger());
}

static void test_write_to(void)
{
    const char * msg = "THIS IS A TEST MESSAGE";
    ppe_log log = NULL;

    log = ppe_log_create(4096, PPE_LOG_INFO, PPE_LOG_SHORT_FILENAME, test_general_write_cfn, test_general_flush_cfn);
    CU_ASSERT_PTR_NOT_NULL_FATAL(log);

    ppe_log_write_to(&log, "TEST_WRITE_TO", PPE_LOG_INFO, msg, strlen(msg));
    CU_ASSERT_EQUAL(test_verify_timestamp(log_buf), 19);
    CU_ASSERT_PTR_NOT_NULL(strstr(log_buf, "TEST_WRITE_TO"));
    CU_ASSERT_PTR_NOT_NULL(strstr(log_buf, "[INFO]"));
    CU_ASSERT_PTR_NOT_NULL(strstr(log_buf, msg));

    ppe_log_destroy(log);
}

CU_TestInfo test_normal_cases[] = {
    {"test_get_stderr_logger()", test_get_stderr_logger},
    {"test_get_global_default()", test_get_global_default},
    {"test_set_global_default()", test_set_global_default},
    {"test_get_threshold()", test_get_threshold},
    {"test_set_threshold()", test_set_threshold},
    {"test_get_flags()", test_get_flags},
    {"test_set_flags()", test_set_flags},
    {"test_create()", test_create},
    {"test_destroy()", test_destroy},
    {"test_write_to()", test_write_to},
    CU_TEST_INFO_NULL
};

CU_SuiteInfo suites[] = {
    {"test_normal_cases", NULL, NULL, test_normal_cases},
    CU_SUITE_INFO_NULL
};

int main(void)
{
    CU_pSuite pSuite = NULL;

    if (CU_initialize_registry() != CUE_SUCCESS) {
        return CU_get_error();
    } // if

    pSuite = CU_add_suite("Suite_Test_Log", NULL, NULL);
    if (pSuite == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    } // if

    if (CU_register_suites(suites) != CUE_SUCCESS) {
        printf("Cannot register test suites.\n");
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
