#include <string.h>
#include <stdlib.h>

#include <CUnit/Basic.h> 

#include "ppe/string.h"
#include "ppe/error.h"

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

static void test_cs_substr_for_using_measure_mode(void)
{
    ppe_cstr_c s = "This is a test line.";
    ppe_cstr_c t = NULL;
    ppe_size sz = 0;

    t = ppe_cs_substr(s, 0, 5, NULL, &sz, 0);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, 6);

    t = ppe_cs_substr(s, 10, 5, NULL, &sz, 0);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, 6);

    t = ppe_cs_substr(s, 17, 5, NULL, &sz, 0);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, 4);
} /* test_cs_substr_for_using_measure_mode */

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

static void test_cs_trim_for_using_measure_mode(void)
{
    ppe_cstr_c s = "   Text surrounded by spaces.   ";
    ppe_cstr_c t = NULL;
    ppe_size sz = 0;

    /* -- Test for preceding and/or tailing spaces. -- */
    sz = 0;
    t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, &sz, PPE_STR_OPT_BOTH_ENDS);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, 26 + 1);

    sz = 0;
    t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, &sz, PPE_STR_OPT_LEFT_END);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, 29 + 1);

    sz = 0;
    t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, &sz, PPE_STR_OPT_RIGHT_END);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, 29 + 1);

    /* -- Test for preceding and/or tailing tabs. -- */
    s = "\t\t\tText surrounded by spaces.\t\t\t";

    sz = 0;
    t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, &sz, PPE_STR_OPT_BOTH_ENDS);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, 26 + 1);

    sz = 0;
    t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, &sz, PPE_STR_OPT_LEFT_END);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, 29 + 1);

    sz = 0;
    t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, &sz, PPE_STR_OPT_RIGHT_END);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, 29 + 1);

    /* -- Test for preceding and/or tailing tabs interleaving with spaces. -- */
    s = " \t Text surrounded by spaces.\t \t";

    sz = 0;
    t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, &sz, PPE_STR_OPT_BOTH_ENDS);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, 26 + 1);

    sz = 0;
    t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, &sz, PPE_STR_OPT_LEFT_END);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, 29 + 1);

    sz = 0;
    t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, &sz, PPE_STR_OPT_RIGHT_END);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, 29 + 1);
} /* test_cs_trim_for_using_measure_mode */ 

static void test_cs_trim_for_using_new_string_mode(void)
{
    ppe_cstr_c s = "   Text surrounded by spaces.   ";
    ppe_cstr_c t = NULL;
    ppe_cstr_c t2 = NULL;

    /* -- Test for preceding and/or tailing spaces. -- */
    t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, NULL, PPE_STR_OPT_BOTH_ENDS);
    CU_ASSERT_PTR_NOT_NULL(t);
    CU_ASSERT_PTR_NOT_EQUAL(t, s);
    CU_ASSERT_EQUAL(ppe_cs_size(t), 26);
    CU_ASSERT_EQUAL(strcmp(t, "Text surrounded by spaces."), 0);

    t2 = ppe_cs_trim(s, PPE_STR_SPACES, NULL, NULL, PPE_STR_OPT_LEFT_END);
    CU_ASSERT_PTR_NOT_NULL(t2);
    CU_ASSERT_PTR_NOT_EQUAL(t2, s);
    CU_ASSERT_PTR_NOT_EQUAL(t2, t);
    CU_ASSERT_EQUAL(ppe_cs_size(t2), 29);
    CU_ASSERT_EQUAL(strcmp(t2, "Text surrounded by spaces.   "), 0);

    ppe_cs_destroy(t);
    ppe_cs_destroy(t2);

    t2 = ppe_cs_trim(s, PPE_STR_SPACES, NULL, NULL, PPE_STR_OPT_RIGHT_END);
    CU_ASSERT_PTR_NOT_NULL(t2);
    CU_ASSERT_PTR_NOT_EQUAL(t2, s);
    CU_ASSERT_EQUAL(ppe_cs_size(t2), 29);
    CU_ASSERT_EQUAL(strcmp(t2, "   Text surrounded by spaces."), 0);

    ppe_cs_destroy(t2);

    /* -- Test for preceding and/or tailing tabs. -- */
    s = "\t\t\tText surrounded by spaces.\t\t\t";

    t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, NULL, PPE_STR_OPT_BOTH_ENDS);
    CU_ASSERT_PTR_NOT_NULL(t);
    CU_ASSERT_PTR_NOT_EQUAL(t, s);
    CU_ASSERT_EQUAL(ppe_cs_size(t), 26);
    CU_ASSERT_EQUAL(strcmp(t, "Text surrounded by spaces."), 0);

    t2 = ppe_cs_trim(s, PPE_STR_SPACES, NULL, NULL, PPE_STR_OPT_LEFT_END);
    CU_ASSERT_PTR_NOT_NULL(t2);
    CU_ASSERT_PTR_NOT_EQUAL(t2, s);
    CU_ASSERT_PTR_NOT_EQUAL(t2, t);
    CU_ASSERT_EQUAL(ppe_cs_size(t2), 29);
    CU_ASSERT_EQUAL(strcmp(t2, "Text surrounded by spaces.\t\t\t"), 0);

    ppe_cs_destroy(t);
    ppe_cs_destroy(t2);

    t2 = ppe_cs_trim(s, PPE_STR_SPACES, NULL, NULL, PPE_STR_OPT_RIGHT_END);
    CU_ASSERT_PTR_NOT_NULL(t2);
    CU_ASSERT_PTR_NOT_EQUAL(t2, s);
    CU_ASSERT_EQUAL(ppe_cs_size(t2), 29);
    CU_ASSERT_EQUAL(strcmp(t2, "\t\t\tText surrounded by spaces."), 0);

    ppe_cs_destroy(t2);

    /* -- Test for preceding and/or tailing tabs interleaving with spaces. -- */
    s = " \t Text surrounded by spaces.\t \t";

    t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, NULL, PPE_STR_OPT_BOTH_ENDS);
    CU_ASSERT_PTR_NOT_NULL(t);
    CU_ASSERT_PTR_NOT_EQUAL(t, s);
    CU_ASSERT_EQUAL(ppe_cs_size(t), 26);
    CU_ASSERT_EQUAL(strcmp(t, "Text surrounded by spaces."), 0);

    t2 = ppe_cs_trim(s, PPE_STR_SPACES, NULL, NULL, PPE_STR_OPT_LEFT_END);
    CU_ASSERT_PTR_NOT_NULL(t2);
    CU_ASSERT_PTR_NOT_EQUAL(t2, s);
    CU_ASSERT_PTR_NOT_EQUAL(t2, t);
    CU_ASSERT_EQUAL(ppe_cs_size(t2), 29);
    CU_ASSERT_EQUAL(strcmp(t2, "Text surrounded by spaces.\t \t"), 0);

    ppe_cs_destroy(t);
    ppe_cs_destroy(t2);

    t2 = ppe_cs_trim(s, PPE_STR_SPACES, NULL, NULL, PPE_STR_OPT_RIGHT_END);
    CU_ASSERT_PTR_NOT_NULL(t2);
    CU_ASSERT_PTR_NOT_EQUAL(t2, s);
    CU_ASSERT_EQUAL(ppe_cs_size(t2), 29);
    CU_ASSERT_EQUAL(strcmp(t2, " \t Text surrounded by spaces."), 0);

    ppe_cs_destroy(t2);
} /* test_cs_trim_for_using_new_string_mode */

static void test_cs_trim_for_using_fill_buffer_mode(void)
{
    ppe_cstr_c s = "   Text surrounded by spaces.   ";
    ppe_cstr_c t = NULL;
    ppe_char b[30];
    ppe_size sz = sizeof(b);

    /* -- Test for preceding and/or tailing spaces. -- */
    sz = sizeof(b);
    t = ppe_cs_trim(s, PPE_STR_SPACES, b, &sz, PPE_STR_OPT_BOTH_ENDS);
    CU_ASSERT_PTR_NOT_NULL(t);
    CU_ASSERT_PTR_NOT_EQUAL(t, s);
    CU_ASSERT_PTR_EQUAL(t, b);
    CU_ASSERT_EQUAL(ppe_cs_size(b), 26);
    CU_ASSERT_EQUAL(sz, 26);
    CU_ASSERT_EQUAL(strcmp(b, "Text surrounded by spaces."), 0);

    sz = sizeof(b);
    t = ppe_cs_trim(s, PPE_STR_SPACES, b, &sz, PPE_STR_OPT_LEFT_END);
    CU_ASSERT_PTR_NOT_NULL(t);
    CU_ASSERT_PTR_NOT_EQUAL(t, s);
    CU_ASSERT_PTR_EQUAL(t, b);
    CU_ASSERT_EQUAL(ppe_cs_size(b), 29);
    CU_ASSERT_EQUAL(sz, 29);
    CU_ASSERT_EQUAL(strcmp(b, "Text surrounded by spaces.   "), 0);

    sz = sizeof(b);
    t = ppe_cs_trim(s, PPE_STR_SPACES, b, &sz, PPE_STR_OPT_RIGHT_END);
    CU_ASSERT_PTR_NOT_NULL(t);
    CU_ASSERT_PTR_NOT_EQUAL(t, s);
    CU_ASSERT_PTR_EQUAL(t, b);
    CU_ASSERT_EQUAL(ppe_cs_size(b), 29);
    CU_ASSERT_EQUAL(sz, 29);
    CU_ASSERT_EQUAL(strcmp(b, "   Text surrounded by spaces."), 0);

    /* -- Test for preceding and/or tailing tabs. -- */
    s = "\t\t\tText surrounded by spaces.\t\t\t";

    sz = sizeof(b);
    t = ppe_cs_trim(s, PPE_STR_SPACES, b, &sz, PPE_STR_OPT_BOTH_ENDS);
    CU_ASSERT_PTR_NOT_NULL(t);
    CU_ASSERT_PTR_NOT_EQUAL(t, s);
    CU_ASSERT_PTR_EQUAL(t, b);
    CU_ASSERT_EQUAL(ppe_cs_size(b), 26);
    CU_ASSERT_EQUAL(sz, 26);
    CU_ASSERT_EQUAL(strcmp(b, "Text surrounded by spaces."), 0);

    sz = sizeof(b);
    t = ppe_cs_trim(s, PPE_STR_SPACES, b, &sz, PPE_STR_OPT_LEFT_END);
    CU_ASSERT_PTR_NOT_NULL(t);
    CU_ASSERT_PTR_NOT_EQUAL(t, s);
    CU_ASSERT_PTR_EQUAL(t, b);
    CU_ASSERT_EQUAL(ppe_cs_size(b), 29);
    CU_ASSERT_EQUAL(sz, 29);
    CU_ASSERT_EQUAL(strcmp(b, "Text surrounded by spaces.\t\t\t"), 0);

    sz = sizeof(b);
    t = ppe_cs_trim(s, PPE_STR_SPACES, b, &sz, PPE_STR_OPT_RIGHT_END);
    CU_ASSERT_PTR_NOT_NULL(t);
    CU_ASSERT_PTR_NOT_EQUAL(t, s);
    CU_ASSERT_PTR_EQUAL(t, b);
    CU_ASSERT_EQUAL(ppe_cs_size(b), 29);
    CU_ASSERT_EQUAL(sz, 29);
    CU_ASSERT_EQUAL(strcmp(b, "\t\t\tText surrounded by spaces."), 0);

    /* -- Test for preceding and/or tailing tabs interleaving with spaces. -- */
    s = " \t Text surrounded by spaces.\t \t";

    sz = sizeof(b);
    t = ppe_cs_trim(s, PPE_STR_SPACES, b, &sz, PPE_STR_OPT_BOTH_ENDS);
    CU_ASSERT_PTR_NOT_NULL(t);
    CU_ASSERT_PTR_NOT_EQUAL(t, s);
    CU_ASSERT_PTR_EQUAL(t, b);
    CU_ASSERT_EQUAL(ppe_cs_size(b), 26);
    CU_ASSERT_EQUAL(sz, 26);
    CU_ASSERT_EQUAL(strcmp(b, "Text surrounded by spaces."), 0);

    sz = sizeof(b);
    t = ppe_cs_trim(s, PPE_STR_SPACES, b, &sz, PPE_STR_OPT_LEFT_END);
    CU_ASSERT_PTR_NOT_NULL(t);
    CU_ASSERT_PTR_NOT_EQUAL(t, s);
    CU_ASSERT_PTR_EQUAL(t, b);
    CU_ASSERT_EQUAL(ppe_cs_size(b), 29);
    CU_ASSERT_EQUAL(sz, 29);
    CU_ASSERT_EQUAL(strcmp(b, "Text surrounded by spaces.\t \t"), 0);

    sz = sizeof(b);
    t = ppe_cs_trim(s, PPE_STR_SPACES, b, &sz, PPE_STR_OPT_RIGHT_END);
    CU_ASSERT_PTR_NOT_NULL(t);
    CU_ASSERT_PTR_NOT_EQUAL(t, s);
    CU_ASSERT_PTR_EQUAL(t, b);
    CU_ASSERT_EQUAL(ppe_cs_size(b), 29);
    CU_ASSERT_EQUAL(sz, 29);
    CU_ASSERT_EQUAL(strcmp(b, " \t Text surrounded by spaces."), 0);
} /* test_cs_trim_for_using_fill_buffer_mode */

static void test_cs_chop_for_using_measure_mode(void)
{
    ppe_cstr_c s = "This is a test line." PPE_STR_NEWLINE;
    ppe_cstr_c t = NULL;
    ppe_size sz = 0;

    /* -- Test non-empty string input. -- */
    t = ppe_cs_chop(s, NULL, &sz, PPE_STR_OPT_NONE);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, ppe_cs_size(s));

    /* -- Test empty string input. -- */
    t = ppe_cs_chop("", NULL, &sz, PPE_STR_OPT_NONE);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, 1);
} /* test_cs_chop_for_using_measure_mode */

static void test_cs_chop_for_using_new_string_mode(void)
{
    ppe_cstr_c s = "This is a test line." PPE_STR_NEWLINE;
    ppe_cstr_c t = NULL;
    ppe_cstr_c t2 = NULL;

    /* -- Test non-empty string input. -- */
    t = ppe_cs_chop(s, NULL, NULL, PPE_STR_OPT_NONE);
    CU_ASSERT_PTR_NOT_NULL(t);
    CU_ASSERT_PTR_NOT_EQUAL(t, s);
    CU_ASSERT_EQUAL(ppe_cs_size(t), ppe_cs_size(s) - 1);

    t2 = ppe_cs_chop(t, NULL, NULL, PPE_STR_OPT_NONE);
    CU_ASSERT_PTR_NOT_NULL(t2);
    CU_ASSERT_PTR_NOT_EQUAL(t2, t);
    CU_ASSERT_PTR_NOT_EQUAL(t2, s);
    CU_ASSERT_EQUAL(ppe_cs_size(t2), ppe_cs_size(s) - 2);

    ppe_cs_destroy(t);
    ppe_cs_destroy(t2);

    /* -- Test empty string input. -- */
    t = ppe_cs_chop("", NULL, NULL, PPE_STR_OPT_NONE);
    CU_ASSERT_PTR_NOT_NULL(t);
    CU_ASSERT_PTR_NOT_EQUAL(t, s);
    CU_ASSERT_PTR_EQUAL(t, ppe_cs_get_empty());
    CU_ASSERT_EQUAL(ppe_cs_size(t), 0);
} /* test_cs_chop_for_using_new_string_mode */

static void test_cs_chop_for_using_fill_buffer_mode(void)
{
    ppe_cstr_c s = "This is a test line." PPE_STR_NEWLINE;
    ppe_cstr_c t = NULL;
    ppe_cstr_c t2 = NULL;
    ppe_char b[30];
    ppe_size sz = sizeof(b);

    /* -- Test non-empty string input. -- */
    t = ppe_cs_chop(s, NULL, NULL, PPE_STR_OPT_NONE);
    t = ppe_cs_chop(s, b, &sz, PPE_STR_OPT_NONE);
    CU_ASSERT_PTR_NOT_NULL(t);
    CU_ASSERT_PTR_NOT_EQUAL(t, s);
    CU_ASSERT_PTR_EQUAL(t, b);
    CU_ASSERT_EQUAL(sz, ppe_cs_size(s) - 1);
    CU_ASSERT_EQUAL(strncmp(t, s, sz), 0);

    sz = sizeof(b);
    t2 = ppe_cs_chop(t, b, &sz, PPE_STR_OPT_NONE);
    CU_ASSERT_PTR_NOT_NULL(t2);
    CU_ASSERT_PTR_NOT_EQUAL(t2, s);
    CU_ASSERT_PTR_EQUAL(t2, b);
    CU_ASSERT_EQUAL(sz, ppe_cs_size(s) - 2);
    CU_ASSERT_EQUAL(strncmp(t2, s, sz), 0);

    /* -- Test empty string input. -- */
    t = ppe_cs_chop("", NULL, NULL, PPE_STR_OPT_NONE);
    t = ppe_cs_chop("", b, &sz, PPE_STR_OPT_NONE);
    CU_ASSERT_PTR_NOT_NULL(t);
    CU_ASSERT_PTR_NOT_EQUAL(t, s);
    CU_ASSERT_PTR_EQUAL(t, b);
    CU_ASSERT_EQUAL(sz, 0);
    CU_ASSERT_EQUAL(b[0], '\0');
} /* test_cs_chop_for_using_fill_buffer_mode */

static void test_cs_chomp_for_using_measure_mode(void)
{
    ppe_cstr_c s = "This is a test line." PPE_STR_NEWLINE "And this is another one." PPE_STR_NEWLINE PPE_STR_NEWLINE;
    ppe_cstr_c t = NULL;
    ppe_size sz = 0;

    /* -- Test for non-empty string input. -- */
    /*  Test for stripping out one newline.  */
    sz = 0;
    t = ppe_cs_chomp(s, PPE_STR_NEWLINE, 1, NULL, &sz, PPE_STR_OPT_NONE);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, strlen(s) - strlen(PPE_STR_NEWLINE) + 1);

    /*  Test for stripping out two newlines.  */
    sz = 0;
    t = ppe_cs_chomp(s, PPE_STR_NEWLINE, 2, NULL, &sz, PPE_STR_OPT_NONE);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, strlen(s) - strlen(PPE_STR_NEWLINE) * 2 + 1);

    sz = 0;
    t = ppe_cs_chomp(s, PPE_STR_NEWLINE, 3, NULL, &sz, PPE_STR_OPT_NONE);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, strlen(s) - strlen(PPE_STR_NEWLINE) * 2 + 1);

    sz = 0;
    t = ppe_cs_chomp(s, PPE_STR_NEWLINE, -1, NULL, &sz, PPE_STR_OPT_NONE);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, strlen(s) - strlen(PPE_STR_NEWLINE) * 2 + 1);

    /* -- Test for empty string input. -- */
    s = "";

    /*  Test for stripping out one newline.  */
    sz = 0;
    t = ppe_cs_chomp(s, PPE_STR_NEWLINE, 1, NULL, &sz, PPE_STR_OPT_NONE);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, 1);

    /*  Test for stripping out two newlines.  */
    sz = 0;
    t = ppe_cs_chomp(s, PPE_STR_NEWLINE, 2, NULL, &sz, PPE_STR_OPT_NONE);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, 1);

    sz = 0;
    t = ppe_cs_chomp(s, PPE_STR_NEWLINE, 3, NULL, &sz, PPE_STR_OPT_NONE);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, 1);

    sz = 0;
    t = ppe_cs_chomp(s, PPE_STR_NEWLINE, -1, NULL, &sz, PPE_STR_OPT_NONE);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, 1);

    /* -- Test for blank string input. -- */
    s = PPE_STR_NEWLINE PPE_STR_NEWLINE;

    /*  Test for stripping out one newline.  */
    sz = 0;
    t = ppe_cs_chomp(s, PPE_STR_NEWLINE, 1, NULL, &sz, PPE_STR_OPT_NONE);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, strlen(s) - strlen(PPE_STR_NEWLINE) + 1);

    /*  Test for stripping out two newlines.  */
    sz = 0;
    t = ppe_cs_chomp(s, PPE_STR_NEWLINE, 2, NULL, &sz, PPE_STR_OPT_NONE);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, strlen(s) - strlen(PPE_STR_NEWLINE) * 2 + 1);

    sz = 0;
    t = ppe_cs_chomp(s, PPE_STR_NEWLINE, 3, NULL, &sz, PPE_STR_OPT_NONE);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, strlen(s) - strlen(PPE_STR_NEWLINE) * 2 + 1);

    sz = 0;
    t = ppe_cs_chomp(s, PPE_STR_NEWLINE, -1, NULL, &sz, PPE_STR_OPT_NONE);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, strlen(s) - strlen(PPE_STR_NEWLINE) * 2 + 1);

    /* -- Test for interleaving substrings input. -- */
    s = "abcnabcabc";

    sz = 0;
    t = ppe_cs_chomp(s, "abc", 1, NULL, &sz, PPE_STR_OPT_NONE);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, strlen(s) - strlen("abc") + 1);

    sz = 0;
    t = ppe_cs_chomp(s, "abc", 2, NULL, &sz, PPE_STR_OPT_NONE);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, strlen(s) - strlen("abc") * 2 + 1);

    sz = 0;
    t = ppe_cs_chomp(s, "abc", 3, NULL, &sz, PPE_STR_OPT_NONE);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, strlen(s) - strlen("abc") * 2 + 1);

    sz = 0;
    t = ppe_cs_chomp(s, "abc", -1, NULL, &sz, PPE_STR_OPT_NONE);
    CU_ASSERT_PTR_NULL(t);
    CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
    CU_ASSERT_EQUAL(sz, strlen(s) - strlen("abc") * 2 + 1);
} /* test_cs_chomp_for_using_measure_mode */

CU_TestInfo test_normal_cases[] = {
    {"test_cs_get_empty()", test_cs_get_empty},
    {"test_cs_size()", test_cs_size},
    {"test_cs_is_empty()", test_cs_is_empty},
    {"test_cs_compare()", test_cs_compare},
    {"test_cs_find()", test_cs_find},
    {"test_cs_create()", test_cs_create},
    {"test_cs_substr_for_using_measure_mode()", test_cs_substr_for_using_measure_mode},
    {"test_cs_substr_for_using_fill_buffer_mode()", test_cs_substr_for_using_fill_buffer_mode},
    {"test_cs_substr_for_using_new_string_mode()", test_cs_substr_for_using_new_string_mode},
    {"test_cs_trim_for_using_measure_mode()", test_cs_trim_for_using_measure_mode},
    {"test_cs_trim_for_using_fill_buffer_mode()", test_cs_trim_for_using_fill_buffer_mode},
    {"test_cs_trim_for_using_new_string_mode()", test_cs_trim_for_using_new_string_mode},
    {"test_cs_chop_for_using_measure_mode()", test_cs_chop_for_using_measure_mode},
    {"test_cs_chop_for_using_fill_buffer_mode()", test_cs_chop_for_using_fill_buffer_mode},
    {"test_cs_chop_for_using_new_string_mode()", test_cs_chop_for_using_new_string_mode},
    {"test_cs_chomp_for_using_measure_mode()", test_cs_chomp_for_using_measure_mode},
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
