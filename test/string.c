#include <string.h>
#include <stdlib.h>

#include <CUnit/Basic.h> 

#include "ppe/string.h"

static void test_cs_get_empty(void)
{
    ppe_cstr_c s = ppe_cs_get_empty();

    CU_ASSERT_PTR_NOT_NULL(s);
    CU_ASSERT_EQUAL(strlen(s), 0);
}

static void test_cs_size(void)
{
    ppe_cstr_c s = "TEST";
    ppe_size sz = ppe_cs_size(s);

    CU_ASSERT_EQUAL(strlen(s), sz);
}

static void test_cs_is_empty(void)
{
    ppe_cstr_c s = "";
    CU_ASSERT_TRUE(ppe_cs_is_empty(s));

    s = ppe_cs_get_empty();
    CU_ASSERT_TRUE(ppe_cs_is_empty(s));
}

static void test_cs_compare(void)
{
    CU_ASSERT_EQUAL(ppe_cs_compare("aa", "aaa"), -1);
    CU_ASSERT_EQUAL(ppe_cs_compare("aa", "aa"), 0);
    CU_ASSERT_EQUAL(ppe_cs_compare("aaa", "aa"), 1);

    CU_ASSERT_EQUAL(ppe_cs_compare("AAA", "aa"), -1);
    CU_ASSERT_EQUAL(ppe_cs_compare("aa", "AAA"), 1);

    CU_ASSERT_EQUAL(ppe_cs_compare("A", "BBB"), -1);
    CU_ASSERT_EQUAL(ppe_cs_compare("BBB", "A"), 1);

    CU_ASSERT_EQUAL(ppe_cs_compare("AAAA", "B"), -1);
    CU_ASSERT_EQUAL(ppe_cs_compare("B", "AAAA"), 1);
}

static void test_cs_find(void)
{
    ppe_cstr_c s = "This is a test line.";
    ppe_cstr_c r = NULL;

    r = ppe_cs_find(s, "This");
    CU_ASSERT_PTR_NOT_NULL(r);
    CU_ASSERT_EQUAL(r - s, 0);

    r = ppe_cs_find(s, "is is");
    CU_ASSERT_PTR_NOT_NULL(r);
    CU_ASSERT_EQUAL(r - s, 2);

    r = ppe_cs_find(s, "line.");
    CU_ASSERT_PTR_NOT_NULL(r);
    CU_ASSERT_EQUAL(r - s, 15);

    r = ppe_cs_find(s, ". It");
    CU_ASSERT_PTR_NULL(r);

    r = ppe_cs_find(s, "What");
    CU_ASSERT_PTR_NULL(r);
}

/*
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
*/

CU_TestInfo test_normal_cases[] = {
    {"test_cs_get_empty()", test_cs_get_empty},
    {"test_cs_size()", test_cs_size},
    {"test_cs_is_empty()", test_cs_is_empty},
    {"test_cs_compare()", test_cs_compare},
    {"test_cs_find()", test_cs_find},
    CU_TEST_INFO_NULL
};

CU_SuiteInfo suites[] = {
    {"test_normal_cases", NULL, NULL, NULL, NULL, test_normal_cases},
    CU_SUITE_INFO_NULL
};

int main(void)
{
    CU_pSuite pSuite = NULL;

    if (CU_initialize_registry() != CUE_SUCCESS) {
        return CU_get_error();
    } /* if */

    pSuite = CU_add_suite("Suite_Test_Log", NULL, NULL);
    if (pSuite == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    } /* if */

    if (CU_register_suites(suites) != CUE_SUCCESS) {
        printf("Cannot register test suites.\n");
        CU_cleanup_registry();
        return CU_get_error();
    } /* if */
    
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
