#include <string.h>
#include <stdlib.h>

#include <CUnit/Basic.h> 

#include "ppe/string.h"

static void test_str_get_empty(void)
{
    ppe_string str = ppe_str_get_empty();
    CU_ASSERT_PTR_NOT_NULL(str);
    CU_ASSERT_EQUAL(strlen(ppe_str_cstr(str)), 0);
    CU_ASSERT_EQUAL(ppe_str_size(str), 0);
}

static void test_cs_clone(void)
{
    const char * src = "TEST LINE";
    ppe_string str = NULL;
    ppe_string str2 = NULL;

    str = ppe_cs_clone("", 0);
    CU_ASSERT_EQUAL(str, ppe_str_get_empty());
    CU_ASSERT_EQUAL(strlen(ppe_str_cstr(str)), 0);
    CU_ASSERT_EQUAL(ppe_str_size(str), 0);

    str = ppe_cs_clone(ppe_str_cstr(ppe_str_get_empty()), 0);
    CU_ASSERT_EQUAL(str, ppe_str_get_empty());
    CU_ASSERT_EQUAL(strlen(ppe_str_cstr(str)), 0);
    CU_ASSERT_EQUAL(ppe_str_size(str), 0);

    str = ppe_cs_clone("", PPE_STR_AUTO_LENGTH);
    CU_ASSERT_EQUAL(str, ppe_str_get_empty());
    CU_ASSERT_EQUAL(strlen(ppe_str_cstr(str)), 0);
    CU_ASSERT_EQUAL(ppe_str_size(str), 0);

    str = ppe_cs_clone(src, PPE_STR_AUTO_LENGTH);
    CU_ASSERT_NOT_EQUAL(str, ppe_str_get_empty());
    CU_ASSERT_EQUAL(strlen(ppe_str_cstr(str)), strlen(src));
    CU_ASSERT_EQUAL(ppe_str_size(str), strlen(src));
    CU_ASSERT_EQUAL(strncmp(ppe_str_cstr(str), src, strlen(src)), 0);

    str2 = ppe_cs_clone(src, strlen(src));
    CU_ASSERT_NOT_EQUAL(str2, ppe_str_get_empty());
    CU_ASSERT_NOT_EQUAL(str2, str);
    CU_ASSERT_EQUAL(strlen(ppe_str_cstr(str2)), strlen(src));
    CU_ASSERT_EQUAL(ppe_str_size(str2), strlen(src));
    CU_ASSERT_EQUAL(strncmp(ppe_str_cstr(str), src, strlen(src)), 0);

    ppe_mp_free(str2);
    ppe_mp_free(str);
}

static void test_str_destroy(void)
{
    const char * src = "TEST LINE";
    ppe_string str = NULL;

    ppe_str_destroy(ppe_str_get_empty());

    str = ppe_cs_clone(src, strlen(src));
    CU_ASSERT_PTR_NOT_NULL(str);
    CU_ASSERT_NOT_EQUAL(str, ppe_str_get_empty());

    ppe_str_destroy(str);
}

static void test_cs_join_2(void)
{
    const char * src1 = "TEST";
    const char * src2 = "LINE";
    const char * ret1 = "TEST LINE"; 
    const char * ret2 = "TEST::LINE"; 
    const char * ret3 = "TEST\tLINE"; 
    const char * ret4 = "TESTLINE"; 
    ppe_string str = NULL;

    str = ppe_cs_join_2(" ", src1, strlen(src1), src2, strlen(src2));
    CU_ASSERT_PTR_NOT_NULL(str);
    CU_ASSERT_NOT_EQUAL(str, ppe_str_get_empty());
    CU_ASSERT_EQUAL(ppe_str_size(str), strlen(ret1));
    CU_ASSERT_EQUAL(strcmp(ppe_str_cstr(str), ret1), 0);

    ppe_str_destroy(str);

    str = ppe_cs_join_2(" ", src1, PPE_STR_AUTO_LENGTH, src2, PPE_STR_AUTO_LENGTH);
    CU_ASSERT_PTR_NOT_NULL(str);
    CU_ASSERT_NOT_EQUAL(str, ppe_str_get_empty());
    CU_ASSERT_EQUAL(ppe_str_size(str), strlen(ret1));
    CU_ASSERT_EQUAL(strcmp(ppe_str_cstr(str), ret1), 0);

    ppe_str_destroy(str);

    str = ppe_cs_join_2("::", src1, strlen(src1), src2, PPE_STR_AUTO_LENGTH);
    CU_ASSERT_PTR_NOT_NULL(str);
    CU_ASSERT_NOT_EQUAL(str, ppe_str_get_empty());
    CU_ASSERT_EQUAL(ppe_str_size(str), strlen(ret2));
    CU_ASSERT_EQUAL(strcmp(ppe_str_cstr(str), ret2), 0);

    ppe_str_destroy(str);

    str = ppe_cs_join_2("\t", src1, PPE_STR_AUTO_LENGTH, src2, strlen(src2));
    CU_ASSERT_PTR_NOT_NULL(str);
    CU_ASSERT_NOT_EQUAL(str, ppe_str_get_empty());
    CU_ASSERT_EQUAL(ppe_str_size(str), strlen(ret3));
    CU_ASSERT_EQUAL(strcmp(ppe_str_cstr(str), ret3), 0);

    ppe_str_destroy(str);

    str = ppe_cs_join_2("", src1, strlen(src1), src2, strlen(src2));
    CU_ASSERT_PTR_NOT_NULL(str);
    CU_ASSERT_NOT_EQUAL(str, ppe_str_get_empty());
    CU_ASSERT_EQUAL(ppe_str_size(str), strlen(ret4));
    CU_ASSERT_EQUAL(strcmp(ppe_str_cstr(str), ret4), 0);

    ppe_str_destroy(str);
}

static void test_cs_concat_2(void)
{
    const char * src1 = "TEST";
    const char * src2 = "LINE";
    const char * src3 = "words";
    const char * ret1 = "TESTLINE"; 
    const char * ret2 = "TESTwords"; 
    ppe_string str = NULL;

    str = ppe_cs_concat_2(src1, strlen(src1), src2, strlen(src2));
    CU_ASSERT_PTR_NOT_NULL(str);
    CU_ASSERT_NOT_EQUAL(str, ppe_str_get_empty());
    CU_ASSERT_EQUAL(ppe_str_size(str), strlen(ret1));
    CU_ASSERT_EQUAL(strcmp(ppe_str_cstr(str), ret1), 0);

    ppe_str_destroy(str);

    str = ppe_cs_concat_2(src1, strlen(src1), src3, strlen(src3));
    CU_ASSERT_PTR_NOT_NULL(str);
    CU_ASSERT_NOT_EQUAL(str, ppe_str_get_empty());
    CU_ASSERT_EQUAL(ppe_str_size(str), strlen(ret2));
    CU_ASSERT_EQUAL(strcmp(ppe_str_cstr(str), ret2), 0);

    ppe_str_destroy(str);
}

static void test_cs_join(void)
{
    const char * src1 = "TEST";
    const char * src2 = "LINE";
    const char * src3 = "text";
    const char src4 = {"sentence"};
    const char * ret1 = "TESTLINEtext"; 
    const char * ret2 = "text\tLINE\tTEST"; 
    const char * ret3 = "sentence---TEST---text---LINE"; 
    ppe_string str = NULL;

    str = ppe_cs_join("", src1, strlen(src1), src2, strlen(src2), src3, strlen(src3), PPE_STR_ARG_END);
    CU_ASSERT_PTR_NOT_NULL(str);
    CU_ASSERT_NOT_EQUAL(str, ppe_str_get_empty());
    CU_ASSERT_EQUAL(ppe_str_size(str), strlen(ret1));
    CU_ASSERT_EQUAL(strcmp(ppe_str_cstr(str), ret1), 0);

    ppe_str_destroy(str);

    str = ppe_cs_join("\t", src3, strlen(src3), src2, strlen(src2), src1, strlen(src1), PPE_STR_ARG_END);
    CU_ASSERT_PTR_NOT_NULL(str);
    CU_ASSERT_NOT_EQUAL(str, ppe_str_get_empty());
    CU_ASSERT_EQUAL(ppe_str_size(str), strlen(ret2));
    CU_ASSERT_EQUAL(strcmp(ppe_str_cstr(str), ret2), 0);

    ppe_str_destroy(str);

    str = ppe_cs_join("---", src4, strlen(src4), src1, strlen(src1), src3, strlen(src3), src2, strlen(src2), PPE_STR_ARG_END);
    CU_ASSERT_PTR_NOT_NULL(str);
    CU_ASSERT_NOT_EQUAL(str, ppe_str_get_empty());
    CU_ASSERT_EQUAL(ppe_str_size(str), strlen(ret3));
    CU_ASSERT_EQUAL(strcmp(ppe_str_cstr(str), ret3), 0);

    ppe_str_destroy(str);
}

static void test_cs_join_big(void)
{
    ppe_size buf_cap = 4096 - sizeof(void*) - sizeof(ppe_size) - sizeof(ppe_size);
    ppe_size src1_len = buf_cap - 1;
    ppe_size src2_len = buf_cap;
    ppe_size src3_len = buf_cap + 1;
    ppe_size src4_len = 4096;
    const char * src1 = NULL;
    const char * src2 = NULL;
    const char * src3 = NULL;
    const char * src4 = NULL;
    ppe_string str = NULL;

    src1 = malloc(src1_len);
    CU_ASSERT_PTR_NOT_NULL_FATAL(src1);
    memset(src1, 'A', src1_len);

    src2 = malloc(src2_len);
    CU_ASSERT_PTR_NOT_NULL_FATAL(src2);
    memset(src2, 'B', src2_len);

    src3 = malloc(src3_len);
    CU_ASSERT_PTR_NOT_NULL_FATAL(src3);
    memset(src3, 'C', src3_len);

    src4 = malloc(src4_len);
    CU_ASSERT_PTR_NOT_NULL_FATAL(src4);
    memset(src4, 'D', src4_len);

    /* Data do not go across the end of the first buffer. */

    str = ppe_cs_join("", src1, src1_len, "", 0, PPE_STR_ARG_END);
    CU_ASSERT_PTR_NOT_NULL(str);
    CU_ASSERT_NOT_EQUAL(str, ppe_str_get_empty());
    CU_ASSERT_EQUAL(ppe_str_size(str), src1_len);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str), src1, src1_len), 0);

    ppe_str_destroy(str);

    /* Data reach out to the end of the first buffer. */

    str = ppe_cs_join("", src2, src2_len, "", 0, PPE_STR_ARG_END);
    CU_ASSERT_PTR_NOT_NULL(str);
    CU_ASSERT_NOT_EQUAL(str, ppe_str_get_empty());
    CU_ASSERT_EQUAL(ppe_str_size(str), src2_len);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str), src2, src2_len), 0);

    ppe_str_destroy(str);

    /* Data go across the end of the first buffer. */

    str = ppe_cs_join("", src3, src3_len, "", 0, PPE_STR_ARG_END);
    CU_ASSERT_PTR_NOT_NULL(str);
    CU_ASSERT_NOT_EQUAL(str, ppe_str_get_empty());
    CU_ASSERT_EQUAL(ppe_str_size(str), src3_len);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str), src3, src3_len), 0);

    ppe_str_destroy(str);

    /* Delimiter does not go across the end of the first buffer. */

    /* src1 \t | src1 */

    str = ppe_cs_join("\t", src1, src1_len, src1, src1_len, PPE_STR_ARG_END);
    CU_ASSERT_PTR_NOT_NULL(str);
    CU_ASSERT_NOT_EQUAL(str, ppe_str_get_empty());
    CU_ASSERT_EQUAL(ppe_str_size(str), src1_len + 1 + src1_len);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str), src1, src1_len), 0);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str) + src1_len, "\t", 1), 0);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str) + src1_len + 1, src1, src1_len), 0);

    ppe_str_destroy(str);

    /* src1 \t | src2 */

    str = ppe_cs_join("\t", src1, src1_len, src2, src2_len, PPE_STR_ARG_END);
    CU_ASSERT_PTR_NOT_NULL(str);
    CU_ASSERT_NOT_EQUAL(str, ppe_str_get_empty());
    CU_ASSERT_EQUAL(ppe_str_size(str), src1_len + 1 + src2_len);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str), src1, src1_len), 0);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str) + src1_len, "\t", 1), 0);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str) + src1_len + 1, src2, src2_len), 0);

    ppe_str_destroy(str);

    /* src1 \t | src2 \t <src3 | src3> */

    str = ppe_cs_join("\t", src1, src1_len, src2, src2_len, src3, src3_len, PPE_STR_ARG_END);
    CU_ASSERT_PTR_NOT_NULL(str);
    CU_ASSERT_NOT_EQUAL(str, ppe_str_get_empty());
    CU_ASSERT_EQUAL(ppe_str_size(str), src1_len + 1 + src2_len + 1 + src3_len);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str), src1, src1_len), 0);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str) + src1_len, "\t", 1), 0);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str) + src1_len + 1, src2, src2_len), 0);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str) + src1_len + 1 + src2_len, "\t", 1), 0);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str) + src1_len + 1 + src2_len + 1, src3, src3_len), 0);

    ppe_str_destroy(str);

    /* src1 \t | src4 | \t src2 */

    str = ppe_cs_join("\t", src1, src1_len, src4, src4_len, src2, src2_len, PPE_STR_ARG_END);
    CU_ASSERT_PTR_NOT_NULL(str);
    CU_ASSERT_NOT_EQUAL(str, ppe_str_get_empty());
    CU_ASSERT_EQUAL(ppe_str_size(str), src1_len + 1 + src4_len + 1 + src2_len);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str), src1, src1_len), 0);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str) + src1_len, "\t", 1), 0);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str) + src1_len + 1, src4, src4_len), 0);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str) + src1_len + 1 + src4_len, "\t", 1), 0);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str) + src1_len + 1 + src4_len + 1, src2, src2_len), 0);

    ppe_str_destroy(str);

    /* Delimiter goes across the end of the first buffer. */

    /* src1 = | = src1 */

    str = ppe_cs_join("==", src1, src1_len, src1, src1_len, PPE_STR_ARG_END);
    CU_ASSERT_PTR_NOT_NULL(str);
    CU_ASSERT_NOT_EQUAL(str, ppe_str_get_empty());
    CU_ASSERT_EQUAL(ppe_str_size(str), src1_len + 2 + src1_len);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str), src1, src1_len), 0);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str) + src1_len, "==", 2), 0);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str) + src1_len + 2, src1, src1_len), 0);

    ppe_str_destroy(str);

    /* src1 = | = <src4 | src4> */

    str = ppe_cs_join("==", src1, src1_len, src4, src4_len, PPE_STR_ARG_END);
    CU_ASSERT_PTR_NOT_NULL(str);
    CU_ASSERT_NOT_EQUAL(str, ppe_str_get_empty());
    CU_ASSERT_EQUAL(ppe_str_size(str), src1_len + 2 + src1_len);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str), src1, src1_len), 0);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str) + src1_len, "==", 2), 0);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str) + src1_len + 2, src4, src4_len), 0);

    ppe_str_destroy(str);

    /* Delimiter begins at the start of the second buffer. */

    /* src2 | */

    str = ppe_cs_join("::", src2, src2_len, "", 0, PPE_STR_ARG_END);
    CU_ASSERT_PTR_NOT_NULL(str);
    CU_ASSERT_NOT_EQUAL(str, ppe_str_get_empty());
    CU_ASSERT_EQUAL(ppe_str_size(str), src2_len);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str), src2, src2_len), 0);

    ppe_str_destroy(str);

    /* src2 | :: <src4 | src4> */

    str = ppe_cs_join("::", src2, src2_len, src4, src4_len, PPE_STR_ARG_END);
    CU_ASSERT_PTR_NOT_NULL(str);
    CU_ASSERT_NOT_EQUAL(str, ppe_str_get_empty());
    CU_ASSERT_EQUAL(ppe_str_size(str), src2_len + 2 + src4_len);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str), src2, src2_len), 0);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str) + src2_len, "::", 2), 0);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str) + src2_len + 2, src4, src4_len), 0);

    /* src2 | @@ @@ src3 */

    str = ppe_cs_join("@@", src2, src2_len, "", 0, src3, src3_len, PPE_STR_ARG_END);
    CU_ASSERT_PTR_NOT_NULL(str);
    CU_ASSERT_NOT_EQUAL(str, ppe_str_get_empty());
    CU_ASSERT_EQUAL(ppe_str_size(str), src2_len + 4 + src4_len);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str), src2, src2_len), 0);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str) + src2_len, "@@@@", 4), 0);
    CU_ASSERT_EQUAL(memcmp(ppe_str_cstr(str) + src2_len + 4, src3, src3_len), 0);

    ppe_str_destroy(str);
}

static void test_str_clone(void)
{
    ppe_string str = NULL;

    str = ppe_str_clone(ppe_str_get_empty());
    CU_ASSERT_EQUAL(str, ppe_str_get_empty());
    CU_ASSERT_EQUAL(strlen(ppe_str_cstr(str)), 0);
    CU_ASSERT_EQUAL(ppe_str_size(str), 0);

    ppe_str_destroy(str);
}

CU_TestInfo test_normal_cases[] = {
    {"test_str_get_empty()", test_str_get_empty},
    {"test_cs_clone()", test_cs_clone},
    {"test_str_destroy()", test_str_destroy},
    {"test_cs_join_2()", test_cs_join_2},
    {"test_cs_concat_2()", test_cs_concat_2},
    {"test_cs_join()", test_cs_join},
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
