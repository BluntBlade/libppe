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

static void test_cs_create(void)
{
    ppe_char b[] = "This is a test line.";
    ppe_cstr_c s = NULL;
    ppe_cstr_c s2 = NULL;

    s = ppe_cs_create("", 0);
    CU_ASSERT_PTR_NOT_NULL(s);
    CU_ASSERT_PTR_EQUAL(s, ppe_cs_get_empty());
    CU_ASSERT_EQUAL(ppe_cs_size(s), 0);

    ppe_cs_destroy(s);

    s = ppe_cs_create("", 0);
    CU_ASSERT_PTR_NOT_NULL(s);
    CU_ASSERT_PTR_EQUAL(s, ppe_cs_get_empty());
    CU_ASSERT_EQUAL(ppe_cs_size(s), 0);

    ppe_cs_destroy(s);

    s = ppe_cs_create(b, strlen(b));
    CU_ASSERT_PTR_NOT_NULL(s);
    CU_ASSERT_PTR_NOT_EQUAL(s, ppe_cs_get_empty());
    CU_ASSERT_EQUAL(ppe_cs_size(s), strlen(b));
    CU_ASSERT_EQUAL(strncmp(s, b, strlen(b)), 0);

    s2 = ppe_cs_create(b, strlen(b));
    CU_ASSERT_PTR_NOT_NULL(s2);
    CU_ASSERT_PTR_NOT_EQUAL(s2, ppe_cs_get_empty());
    CU_ASSERT_EQUAL(ppe_cs_size(s2), strlen(b));
    CU_ASSERT_EQUAL(strncmp(s2, b, strlen(b)), 0);
    CU_ASSERT_PTR_NOT_EQUAL(s2, s);
    CU_ASSERT_EQUAL(strncmp(s2, s, ppe_cs_size(s)), 0);

    ppe_cs_destroy(s);
    ppe_cs_destroy(s2);
} /* test_cs_create */

static void test_cs_substr_for_using_new_string_mode(void)
{
    ppe_cstr_c s = "This is a test line.";
    ppe_cstr_c t = NULL;
    ppe_cstr_c t2 = NULL;
    
    /* -- Test NEW-STRING MODE -- */
    /* At the beginning. */
    t = ppe_cs_substr(s, 0, 5, NULL, NULL, 0);
    CU_ASSERT_PTR_NOT_NULL(t);
    CU_ASSERT_PTR_NOT_EQUAL(t, s);
    CU_ASSERT_EQUAL(strncmp(t, s, 5), 0);
    CU_ASSERT_EQUAL(strncmp(t, "This ", 5), 0);

    /* At the middle. */
    t2 = ppe_cs_substr(s, 10, 5, NULL, NULL, 0);
    CU_ASSERT_PTR_NOT_NULL(t2);
    CU_ASSERT_PTR_NOT_EQUAL(t2, s);
    CU_ASSERT_PTR_NOT_EQUAL(t2, t);
    CU_ASSERT_EQUAL(strncmp(t2, s + 10, 5), 0);
    CU_ASSERT_EQUAL(strncmp(t2, "test ", 5), 0);

    ppe_cs_destroy(t);
    ppe_cs_destroy(t2);

    /* At the end. */
    t = ppe_cs_substr(s, 15, 5, NULL, NULL, 0);
    CU_ASSERT_PTR_NOT_NULL(t);
    CU_ASSERT_PTR_NOT_EQUAL(t, s);
    CU_ASSERT_EQUAL(strncmp(t, s + 15, 5), 0);
    CU_ASSERT_EQUAL(strncmp(t, "line.", 5), 0);

    ppe_cs_destroy(t);

    /* Reach beyond the end. */
    t = ppe_cs_substr(s, 17, 5, NULL, NULL, 0);
    CU_ASSERT_PTR_NOT_NULL(t);
    CU_ASSERT_PTR_NOT_EQUAL(t, s);
    CU_ASSERT_EQUAL(strncmp(t, s + 17, 3), 0);
    CU_ASSERT_EQUAL(strncmp(t, "ne.", 3), 0);

    ppe_cs_destroy(t);
} /* test_cs_substr_for_using_new_string_mode */

static void test_cs_substr_for_using_fill_buffer_mode(void)
{
    ppe_cstr_c s = "This is a test line.";
    ppe_cstr_c t = NULL;
    ppe_char b[10];
    ppe_size bsz = sizeof(b);

    /* -- Test FILL-BUFFER MODE -- */
    /* At the beginning. */
    bsz = sizeof(b);
    t = ppe_cs_substr(s, 0, 5, b, &bsz, 0);
    CU_ASSERT_PTR_NOT_NULL(t);
    CU_ASSERT_PTR_EQUAL(t, b);
    CU_ASSERT_EQUAL(strncmp(t, s, 5), 0);
    CU_ASSERT_EQUAL(strncmp(t, "This ", 5), 0);

    /* At the middle. */
    bsz = sizeof(b);
    t = ppe_cs_substr(s, 10, 5, b, &bsz, 0);
    CU_ASSERT_PTR_NOT_NULL(t);
    CU_ASSERT_PTR_EQUAL(t, b);
    CU_ASSERT_EQUAL(strncmp(t, s + 10, 5), 0);
    CU_ASSERT_EQUAL(strncmp(t, "test ", 5), 0);

    /* At the end. */
    bsz = sizeof(b);
    t = ppe_cs_substr(s, 15, 5, b, &bsz, 0);
    CU_ASSERT_PTR_NOT_NULL(t);
    CU_ASSERT_PTR_EQUAL(t, b);
    CU_ASSERT_EQUAL(strncmp(t, s + 15, 5), 0);
    CU_ASSERT_EQUAL(strncmp(t, "line.", 5), 0);

    /* Reach beyond the end. */
    bsz = sizeof(b);
    t = ppe_cs_substr(s, 17, 5, b, &bsz, 0);
    CU_ASSERT_PTR_NOT_NULL(t);
    CU_ASSERT_PTR_EQUAL(t, b);
    CU_ASSERT_EQUAL(strncmp(t, s + 17, 3), 0);
    CU_ASSERT_EQUAL(strncmp(t, "ne.", 3), 0);

    /* Truncate the substring. */
    bsz = sizeof(b);
    t = ppe_cs_substr(s, 5, 10, b, &bsz, 0);
    CU_ASSERT_PTR_NOT_NULL(t);
    CU_ASSERT_PTR_EQUAL(t, b);
    CU_ASSERT_EQUAL(strncmp(t, s + 5, 9), 0);
    CU_ASSERT_EQUAL(strncmp(t, "is a test", 9), 0);
} /* test_cs_substr_for_using_fill_buffer_mode */

CU_TestInfo test_normal_cases[] = {
    {"test_cs_get_empty()", test_cs_get_empty},
    {"test_cs_size()", test_cs_size},
    {"test_cs_is_empty()", test_cs_is_empty},
    {"test_cs_compare()", test_cs_compare},
    {"test_cs_find()", test_cs_find},
    {"test_cs_create()", test_cs_create},
    {"test_cs_substr_for_using_new_string_mode()", test_cs_substr_for_using_new_string_mode},
    {"test_cs_substr_for_using_fill_buffer_mode()", test_cs_substr_for_using_fill_buffer_mode},
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
