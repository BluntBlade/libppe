#include <string.h>
#include <stdlib.h>

#include <CUnit/Basic.h> 

#include "ppe/string.h"
#include "ppe/error.h"

static void test_cs_empty(void)
{
    ppe_cstr_c s = ppe_cs_empty();

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

    s = ppe_cs_empty();
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
    CU_ASSERT_PTR_EQUAL(s, ppe_cs_empty());
    CU_ASSERT_EQUAL(ppe_cs_size(s), 0);

    ppe_cs_destroy(s);

    s = ppe_cs_create("", 0);
    CU_ASSERT_PTR_NOT_NULL(s);
    CU_ASSERT_PTR_EQUAL(s, ppe_cs_empty());
    CU_ASSERT_EQUAL(ppe_cs_size(s), 0);

    ppe_cs_destroy(s);

    s = ppe_cs_create(b, strlen(b));
    CU_ASSERT_PTR_NOT_NULL(s);
    CU_ASSERT_PTR_NOT_EQUAL(s, ppe_cs_empty());
    CU_ASSERT_EQUAL(ppe_cs_size(s), strlen(b));
    CU_ASSERT_EQUAL(memcmp(s, b, strlen(b)), 0);

    s2 = ppe_cs_create(b, strlen(b));
    CU_ASSERT_PTR_NOT_NULL(s2);
    CU_ASSERT_PTR_NOT_EQUAL(s2, ppe_cs_empty());
    CU_ASSERT_EQUAL(ppe_cs_size(s2), strlen(b));
    CU_ASSERT_EQUAL(memcmp(s2, b, strlen(b)), 0);
    CU_ASSERT_PTR_NOT_EQUAL(s2, s);
    CU_ASSERT_EQUAL(memcmp(s2, s, ppe_cs_size(s)), 0);

    ppe_cs_destroy(s);
    ppe_cs_destroy(s2);
} /* test_cs_create */

static void test_cs_slice(void)
{
    ppe_cstr_c s = "This is a test line.";
    ppe_cstr_c t = NULL;
    ppe_size cpsz = 0;
    ppe_size bsz = 0;
    ppe_char b[32];

    /* CASE-1: Slice substring from the beginning of an empty string. */
    {
        /* CASE-1-1: Zero bytes using MEASURE mode. */
        bsz = sizeof(b);
        t = ppe_cs_slice(ppe_cs_empty(), 0, 0, NULL, &bsz, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, 0);

        /* CASE-1-2: Zero bytes using FILL-BUFFER mode. */
        memset(b, 0, sizeof(b));
        bsz = sizeof(b);
        t = ppe_cs_slice(ppe_cs_empty(), 0, 0, b, &bsz, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, 0);

        /* CASE-1-3: Zero bytes using NEW-STRING mode. */
        t = ppe_cs_slice(ppe_cs_empty(), 0, 0, NULL, NULL, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_EQUAL(t, ppe_cs_empty());

        /* CASE-1-4: 10 bytes using MEASURE mode. */
        bsz = sizeof(b);
        t = ppe_cs_slice(ppe_cs_empty(), 0, 10, NULL, &bsz, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, 0);

        /* CASE-1-5: 10 bytes using FILL-BUFFER mode. */
        memset(b, 0, sizeof(b));
        bsz = sizeof(b);
        t = ppe_cs_slice(ppe_cs_empty(), 0, 10, b, &bsz, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, 0);

        /* CASE-1-6: 10 bytes using NEW-STRING mode. */
        t = ppe_cs_slice(ppe_cs_empty(), 0, 10, NULL, NULL, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_EQUAL(t, ppe_cs_empty());
    }

    /* CASE-2: Slice substring beyond the end of an empty string. */
    {
        /* CASE-2-1: Zero bytes using MEASURE mode. */
        bsz = sizeof(b);
        t = ppe_cs_slice(ppe_cs_empty(), 10, 0, NULL, &bsz, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_OUT_OF_RANGE);

        /* CASE-2-2: Zero bytes using FILL-BUFFER mode. */
        memset(b, 0, sizeof(b));
        bsz = sizeof(b);
        t = ppe_cs_slice(ppe_cs_empty(), 10, 0, b, &bsz, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_OUT_OF_RANGE);

        /* CASE-2-3: Zero bytes using NEW-STRING mode. */
        t = ppe_cs_slice(ppe_cs_empty(), 10, 0, NULL, NULL, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_OUT_OF_RANGE);

        /* CASE-2-4: 10 bytes using MEASURE mode. */
        bsz = sizeof(b);
        t = ppe_cs_slice(ppe_cs_empty(), 10, 10, NULL, &bsz, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_OUT_OF_RANGE);

        /* CASE-2-5: 10 bytes using FILL-BUFFER mode. */
        memset(b, 0, sizeof(b));
        bsz = sizeof(b);
        t = ppe_cs_slice(ppe_cs_empty(), 10, 10, b, &bsz, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_OUT_OF_RANGE);

        /* CASE-2-6: 10 bytes using NEW-STRING mode. */
        t = ppe_cs_slice(ppe_cs_empty(), 10, 10, NULL, NULL, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_OUT_OF_RANGE);
    }

    /* CASE-3: Slice substring from the beginning of a non-empty string. */
    {
        /* CASE-3-1: Zero bytes using MEASURE mode. */
        bsz = sizeof(b);
        t = ppe_cs_slice(s, 0, 0, NULL, &bsz, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, 0);

        /* CASE-3-2: Zero bytes using FILL-BUFFER mode. */
        memset(b, 0, sizeof(b));
        bsz = sizeof(b);
        t = ppe_cs_slice(s, 0, 0, b, &bsz, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, 0);

        /* CASE-3-3: Zero bytes using NEW-STRING mode. */
        t = ppe_cs_slice(s, 0, 0, NULL, NULL, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_EQUAL(t, ppe_cs_empty());

        /* CASE-3-4: One bytes using MEASURE mode. */
        bsz = sizeof(b);
        t = ppe_cs_slice(s, 0, 1, NULL, &bsz, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, 1);

        /* CASE-3-5: One bytes using FILL-BUFFER mode. */
        memset(b, 0, sizeof(b));
        bsz = sizeof(b);
        t = ppe_cs_slice(s, 0, 1, b, &bsz, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, 1);
        CU_ASSERT_EQUAL(memcmp(b, s, 1), 0);

        /* CASE-3-6: One bytes using NEW-STRING mode. */
        t = ppe_cs_slice(s, 0, 1, NULL, NULL, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_TRUE(ppe_cs_equal_to(t, "T"));
        CU_ASSERT_FALSE(ppe_cs_equal_to(t, s));
        ppe_cs_destroy(t);

        /* CASE-3-7: First half of the source using MEASURE mode. */
        cpsz = ppe_cs_size(s) / 2;
        bsz = sizeof(b);
        t = ppe_cs_slice(s, 0, cpsz, NULL, &bsz, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, cpsz);

        /* CASE-3-8: First half of the source using FILL-BUFFER mode. */
        memset(b, 0, sizeof(b));
        bsz = sizeof(b);
        t = ppe_cs_slice(s, 0, cpsz, b, &bsz, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, cpsz);
        CU_ASSERT_EQUAL(memcmp(b, s, cpsz), 0);

        /* CASE-3-9: First half of the source using NEW-STRING mode. */
        t = ppe_cs_slice(s, 0, cpsz, NULL, NULL, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_TRUE(ppe_cs_equal_to(t, "This is a "));
        CU_ASSERT_FALSE(ppe_cs_equal_to(t, s));
        ppe_cs_destroy(t);

        /* CASE-3-10: All of the source using MEASURE mode. */
        cpsz = ppe_cs_size(s);
        bsz = sizeof(b);
        t = ppe_cs_slice(s, 0, cpsz, NULL, &bsz, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, cpsz);

        /* CASE-3-11: All of the source using FILL-BUFFER mode. */
        memset(b, 0, sizeof(b));
        bsz = sizeof(b);
        t = ppe_cs_slice(s, 0, cpsz, b, &bsz, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, cpsz);
        CU_ASSERT_EQUAL(memcmp(b, s, cpsz), 0);

        /* CASE-3-12: All of the source using NEW-STRING mode. */
        t = ppe_cs_slice(s, 0, cpsz, NULL, NULL, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_TRUE(ppe_cs_equal_to(t, s));
        ppe_cs_destroy(t);

        /* CASE-3-13: All of the source plus one byte using MEASURE mode. */
        cpsz = ppe_cs_size(s) + 1;
        bsz = sizeof(b);
        t = ppe_cs_slice(s, 0, cpsz, NULL, &bsz, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, cpsz - 1);

        /* CASE-3-14: All of the source plus one byte using FILL-BUFFER mode. */
        memset(b, 0, sizeof(b));
        bsz = sizeof(b);
        t = ppe_cs_slice(s, 0, cpsz, b, &bsz, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, cpsz - 1);
        CU_ASSERT_EQUAL(memcmp(b, s, cpsz), 0);

        /* CASE-3-15: All of the source plus one byte using NEW-STRING mode. */
        t = ppe_cs_slice(s, 0, cpsz, NULL, NULL, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_TRUE(ppe_cs_equal_to(t, s));
    }

    /* CASE-4: Slice substring from the middle of a non-empty string. */
    {
        /* CASE-4-1: Zero bytes using MEASURE mode. */
        bsz = sizeof(b);
        t = ppe_cs_slice(s, 10, 0, NULL, &bsz, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, 0);

        /* CASE-4-2: Zero bytes using FILL-BUFFER mode. */
        memset(b, 0, sizeof(b));
        bsz = sizeof(b);
        t = ppe_cs_slice(s, 0, 0, b, &bsz, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, 0);

        /* CASE-4-3: Zero bytes using NEW-STRING mode. */
        t = ppe_cs_slice(s, 0, 0, NULL, NULL, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_EQUAL(t, ppe_cs_empty());

        /* CASE-4-4: One bytes using MEASURE mode. */
        bsz = sizeof(b);
        t = ppe_cs_slice(s, 10, 1, NULL, &bsz, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, 1);

        /* CASE-4-5: One bytes using FILL-BUFFER mode. */
        memset(b, 0, sizeof(b));
        bsz = sizeof(b);
        t = ppe_cs_slice(s, 10, 1, b, &bsz, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, 1);
        CU_ASSERT_EQUAL(memcmp(b, s + 10, 1), 0);

        /* CASE-4-6: One bytes using NEW-STRING mode. */
        t = ppe_cs_slice(s, 10, 1, NULL, NULL, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_TRUE(ppe_cs_equal_to(t, "t"));
        CU_ASSERT_FALSE(ppe_cs_equal_to(t, s));
        ppe_cs_destroy(t);

        /* CASE-4-7: First half of the rest of the source using MEASURE mode. */
        cpsz = 5;
        bsz = sizeof(b);
        t = ppe_cs_slice(s, 10, cpsz, NULL, &bsz, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, cpsz);

        /* CASE-4-8: First half of the rest of the source using FILL-BUFFER mode. */
        memset(b, 0, sizeof(b));
        bsz = sizeof(b);
        t = ppe_cs_slice(s, 10, cpsz, b, &bsz, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, cpsz);
        CU_ASSERT_EQUAL(memcmp(b, s + 10, cpsz), 0);

        /* CASE-4-9: First half of the rest of the source using NEW-STRING mode. */
        t = ppe_cs_slice(s, 10, cpsz, NULL, NULL, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_TRUE(ppe_cs_equal_to(t, "test "));
        CU_ASSERT_FALSE(ppe_cs_equal_to(t, s + 10));
        ppe_cs_destroy(t);

        /* CASE-4-10: All of the rest of the source using MEASURE mode. */
        cpsz = 5;
        bsz = sizeof(b);
        t = ppe_cs_slice(s, 15, cpsz, NULL, &bsz, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, cpsz);

        /* CASE-4-11: All of the rest of the source using FILL-BUFFER mode. */
        memset(b, 0, sizeof(b));
        bsz = sizeof(b);
        t = ppe_cs_slice(s, 15, cpsz, b, &bsz, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, cpsz);
        CU_ASSERT_EQUAL(memcmp(b, s + 15, cpsz), 0);

        /* CASE-4-12: All of the rest of the source using NEW-STRING mode. */
        t = ppe_cs_slice(s, 15, cpsz, NULL, NULL, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_TRUE(ppe_cs_equal_to(t, s + 15));
        ppe_cs_destroy(t);

        /* CASE-4-13: All of the rest of the source plus one byte using MEASURE mode. */
        cpsz = 6;
        bsz = sizeof(b);
        t = ppe_cs_slice(s, 15, cpsz, NULL, &bsz, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, cpsz - 1);

        /* CASE-4-14: All of the rest of the source plus one byte using FILL-BUFFER mode. */
        memset(b, 0, sizeof(b));
        bsz = sizeof(b);
        t = ppe_cs_slice(s, 15, cpsz, b, &bsz, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, cpsz - 1);
        CU_ASSERT_EQUAL(memcmp(b, s + 15, cpsz), 0);

        /* CASE-4-15: All of the rest of the source plus one byte using NEW-STRING mode. */
        t = ppe_cs_slice(s, 15, cpsz, NULL, NULL, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_TRUE(ppe_cs_equal_to(t, s + 15));
        ppe_cs_destroy(t);
    }

    /* CASE-5: Slice substring from the end of a non-empty string. */
    {
        /* CASE-5-1: Zero bytes using MEASURE mode. */
        bsz = sizeof(b);
        t = ppe_cs_slice(s, ppe_cs_size(s), 0, NULL, &bsz, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, 0);

        /* CASE-1-2: Zero bytes using FILL-BUFFER mode. */
        memset(b, 0, sizeof(b));
        bsz = sizeof(b);
        t = ppe_cs_slice(s, ppe_cs_size(s), 0, b, &bsz, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, 0);

        /* CASE-1-3: Zero bytes using NEW-STRING mode. */
        t = ppe_cs_slice(s, ppe_cs_size(s), 0, NULL, NULL, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_EQUAL(t, ppe_cs_empty());
    }

    /* CASE-6: Slice substring from beyond the end of a non-empty string. */
    {
        /* CASE-6-1: Zero bytes using MEASURE mode. */
        bsz = sizeof(b);
        t = ppe_cs_slice(s, ppe_cs_size(s) + 1, 0, NULL, &bsz, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_OUT_OF_RANGE);

        /* CASE-6-2: Zero bytes using FILL-BUFFER mode. */
        memset(b, 0, sizeof(b));
        bsz = sizeof(b);
        t = ppe_cs_slice(s, ppe_cs_size(s) + 1, 0, b, &bsz, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_OUT_OF_RANGE);

        /* CASE-6-3: Zero bytes using NEW-STRING mode. */
        t = ppe_cs_slice(s, ppe_cs_size(s) + 1, 0, NULL, NULL, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_OUT_OF_RANGE);
    }
} /* test_cs_slice */

static void test_cs_trim(void)
{
    ppe_cstr_c s = NULL;
    ppe_cstr_c t = NULL;
    ppe_size bsz = 0;
    ppe_char b[32];

    /* Case-1: Trim spaces surrounding text. */
    {
        s = "   Text surrounded by spaces.   ";

        /* Case-1-1: Trim preceding spaces before text using MEASURE mode. */
        bsz = sizeof(b);
        t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, &bsz, PPE_STR_OPT_LEFT_END);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, 29);

        /* Case-1-2: Trim preceding spaces before text using FILL-BUFFER mode. */
        bsz = sizeof(b);
        memset(b, 0, sizeof(b));
        t = ppe_cs_trim(s, PPE_STR_SPACES, b, &bsz, PPE_STR_OPT_LEFT_END);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, 29);
        CU_ASSERT_EQUAL(memcmp(t, "Text surrounded by spaces.   ", bsz), 0);

        /* Case-1-3: Trim preceding spaces before text using NEW-STRING mode. */
        t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, NULL, PPE_STR_OPT_LEFT_END);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_NOT_EQUAL(t, b);
        CU_ASSERT_EQUAL(ppe_cs_size(t), 29);
        CU_ASSERT_TRUE(ppe_cs_equal_to(t, "Text surrounded by spaces.   "));
        ppe_cs_destroy(t);

        /* Case-1-4: Trim tailing spaces after text using MEASURE mode. */
        bsz = sizeof(b);
        t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, &bsz, PPE_STR_OPT_RIGHT_END);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, 29);

        /* Case-1-5: Trim tailing spaces after text using FILL-BUFFER mode. */
        bsz = sizeof(b);
        memset(b, 0, sizeof(b));
        t = ppe_cs_trim(s, PPE_STR_SPACES, b, &bsz, PPE_STR_OPT_RIGHT_END);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, 29);
        CU_ASSERT_EQUAL(memcmp(t, "   Text surrounded by spaces.", bsz), 0);

        /* Case-1-6: Trim tailing spaces after text using NEW-STRING mode. */
        t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, NULL, PPE_STR_OPT_RIGHT_END);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_NOT_EQUAL(t, b);
        CU_ASSERT_EQUAL(ppe_cs_size(t), 29);
        CU_ASSERT_TRUE(ppe_cs_equal_to(t, "   Text surrounded by spaces."));
        ppe_cs_destroy(t);

        /* Case-1-7: Trim all spaces surrounding text using MEASURE mode. */
        bsz = sizeof(b);
        t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, &bsz, PPE_STR_OPT_BOTH_ENDS);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, 26);

        /* Case-1-8: Trim all spaces surrounding text using FILL-BUFFER mode. */
        bsz = sizeof(b);
        memset(b, 0, sizeof(b));
        t = ppe_cs_trim(s, PPE_STR_SPACES, b, &bsz, PPE_STR_OPT_BOTH_ENDS);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, 26);
        CU_ASSERT_EQUAL(memcmp(t, "Text surrounded by spaces.", bsz), 0);

        /* Case-1-9: Trim all spaces surrounding using NEW-STRING mode. */
        t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, NULL, PPE_STR_OPT_BOTH_ENDS);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_NOT_EQUAL(t, b);
        CU_ASSERT_EQUAL(ppe_cs_size(t), 26);
        CU_ASSERT_TRUE(ppe_cs_equal_to(t, "Text surrounded by spaces."));
        ppe_cs_destroy(t);
    }

    /* Case-2: Trim spaces preceding text. */
    {
        s = "   Text surrounded by spaces.";

        /* Case-2-1: Trim preceding spaces before text using MEASURE mode. */
        bsz = sizeof(b);
        t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, &bsz, PPE_STR_OPT_LEFT_END);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, 26);

        /* Case-2-2: Trim preceding spaces before text using FILL-BUFFER mode. */
        bsz = sizeof(b);
        memset(b, 0, sizeof(b));
        t = ppe_cs_trim(s, PPE_STR_SPACES, b, &bsz, PPE_STR_OPT_LEFT_END);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, 26);
        CU_ASSERT_EQUAL(memcmp(t, "Text surrounded by spaces.", bsz), 0);

        /* Case-2-3: Trim preceding spaces before text using NEW-STRING mode. */
        t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, NULL, PPE_STR_OPT_LEFT_END);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_NOT_EQUAL(t, b);
        CU_ASSERT_EQUAL(ppe_cs_size(t), 26);
        CU_ASSERT_TRUE(ppe_cs_equal_to(t, "Text surrounded by spaces."));
        ppe_cs_destroy(t);

        /* Case-2-4: Trim tailing spaces after text using MEASURE mode. */
        bsz = sizeof(b);
        t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, &bsz, PPE_STR_OPT_RIGHT_END);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, 29);

        /* Case-2-5: Trim tailing spaces after text using FILL-BUFFER mode. */
        bsz = sizeof(b);
        memset(b, 0, sizeof(b));
        t = ppe_cs_trim(s, PPE_STR_SPACES, b, &bsz, PPE_STR_OPT_RIGHT_END);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, 29);
        CU_ASSERT_EQUAL(memcmp(t, "   Text surrounded by spaces.", bsz), 0);

        /* Case-2-6: Trim tailing spaces after text using NEW-STRING mode. */
        t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, NULL, PPE_STR_OPT_RIGHT_END);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_NOT_EQUAL(t, b);
        CU_ASSERT_EQUAL(ppe_cs_size(t), 29);
        CU_ASSERT_TRUE(ppe_cs_equal_to(t, "   Text surrounded by spaces."));
        ppe_cs_destroy(t);

        /* Case-2-7: Trim all spaces surrounding text using MEASURE mode. */
        bsz = sizeof(b);
        t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, &bsz, PPE_STR_OPT_BOTH_ENDS);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, 26);

        /* Case-2-8: Trim all spaces surrounding text using FILL-BUFFER mode. */
        bsz = sizeof(b);
        memset(b, 0, sizeof(b));
        t = ppe_cs_trim(s, PPE_STR_SPACES, b, &bsz, PPE_STR_OPT_BOTH_ENDS);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, 26);
        CU_ASSERT_EQUAL(memcmp(t, "Text surrounded by spaces.", bsz), 0);

        /* Case-2-9: Trim all spaces surrounding using NEW-STRING mode. */
        t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, NULL, PPE_STR_OPT_BOTH_ENDS);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_NOT_EQUAL(t, b);
        CU_ASSERT_EQUAL(ppe_cs_size(t), 26);
        CU_ASSERT_TRUE(ppe_cs_equal_to(t, "Text surrounded by spaces."));
        ppe_cs_destroy(t);
    }

    /* Case-3: Trim spaces following text. */
    {
        s = "Text surrounded by spaces.   ";

        /* Case-3-1: Trim preceding spaces before text using MEASURE mode. */
        bsz = sizeof(b);
        t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, &bsz, PPE_STR_OPT_LEFT_END);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, 29);

        /* Case-3-2: Trim preceding spaces before text using FILL-BUFFER mode. */
        bsz = sizeof(b);
        memset(b, 0, sizeof(b));
        t = ppe_cs_trim(s, PPE_STR_SPACES, b, &bsz, PPE_STR_OPT_LEFT_END);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, 29);
        CU_ASSERT_EQUAL(memcmp(t, "Text surrounded by spaces.   ", bsz), 0);

        /* Case-3-3: Trim preceding spaces before text using NEW-STRING mode. */
        t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, NULL, PPE_STR_OPT_LEFT_END);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_NOT_EQUAL(t, b);
        CU_ASSERT_EQUAL(ppe_cs_size(t), 29);
        CU_ASSERT_TRUE(ppe_cs_equal_to(t, "Text surrounded by spaces.   "));
        ppe_cs_destroy(t);

        /* Case-3-4: Trim tailing spaces after text using MEASURE mode. */
        bsz = sizeof(b);
        t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, &bsz, PPE_STR_OPT_RIGHT_END);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, 26);

        /* Case-3-5: Trim tailing spaces after text using FILL-BUFFER mode. */
        bsz = sizeof(b);
        memset(b, 0, sizeof(b));
        t = ppe_cs_trim(s, PPE_STR_SPACES, b, &bsz, PPE_STR_OPT_RIGHT_END);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, 26);
        CU_ASSERT_EQUAL(memcmp(t, "Text surrounded by spaces.", bsz), 0);

        /* Case-3-6: Trim tailing spaces after text using NEW-STRING mode. */
        t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, NULL, PPE_STR_OPT_RIGHT_END);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_NOT_EQUAL(t, b);
        CU_ASSERT_EQUAL(ppe_cs_size(t), 26);
        CU_ASSERT_TRUE(ppe_cs_equal_to(t, "Text surrounded by spaces."));
        ppe_cs_destroy(t);

        /* Case-3-7: Trim all spaces surrounding text using MEASURE mode. */
        bsz = sizeof(b);
        t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, &bsz, PPE_STR_OPT_BOTH_ENDS);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, 26);

        /* Case-3-8: Trim all spaces surrounding text using FILL-BUFFER mode. */
        bsz = sizeof(b);
        memset(b, 0, sizeof(b));
        t = ppe_cs_trim(s, PPE_STR_SPACES, b, &bsz, PPE_STR_OPT_BOTH_ENDS);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, 26);
        CU_ASSERT_EQUAL(memcmp(t, "Text surrounded by spaces.", bsz), 0);

        /* Case-3-9: Trim all spaces surrounding using NEW-STRING mode. */
        t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, NULL, PPE_STR_OPT_BOTH_ENDS);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_NOT_EQUAL(t, b);
        CU_ASSERT_EQUAL(ppe_cs_size(t), 26);
        CU_ASSERT_TRUE(ppe_cs_equal_to(t, "Text surrounded by spaces."));
        ppe_cs_destroy(t);
    }

    /* Case-4: Trim blank characters surrounding text. */
    {
        s = " \t Text surrounded by spaces.\t \t";

        /* Case-4-1: Trim preceding blank characters before text using MEASURE mode. */
        bsz = sizeof(b);
        t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, &bsz, PPE_STR_OPT_LEFT_END);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, 29);

        /* Case-4-2: Trim preceding blank characters before text using FILL-BUFFER mode. */
        bsz = sizeof(b);
        memset(b, 0, sizeof(b));
        t = ppe_cs_trim(s, PPE_STR_SPACES, b, &bsz, PPE_STR_OPT_LEFT_END);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, 29);
        CU_ASSERT_EQUAL(memcmp(t, "Text surrounded by spaces.\t \t", bsz), 0);

        /* Case-4-3: Trim preceding blank characters before text using NEW-STRING mode. */
        t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, NULL, PPE_STR_OPT_LEFT_END);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_NOT_EQUAL(t, b);
        CU_ASSERT_EQUAL(ppe_cs_size(t), 29);
        CU_ASSERT_TRUE(ppe_cs_equal_to(t, "Text surrounded by spaces.\t \t"));
        ppe_cs_destroy(t);

        /* Case-4-4: Trim tailing blank characters after text using MEASURE mode. */
        bsz = sizeof(b);
        t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, &bsz, PPE_STR_OPT_RIGHT_END);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, 29);

        /* Case-4-5: Trim tailing blank characters after text using FILL-BUFFER mode. */
        bsz = sizeof(b);
        memset(b, 0, sizeof(b));
        t = ppe_cs_trim(s, PPE_STR_SPACES, b, &bsz, PPE_STR_OPT_RIGHT_END);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, 29);
        CU_ASSERT_EQUAL(memcmp(t, " \t Text surrounded by spaces.", bsz), 0);

        /* Case-4-6: Trim tailing blank characters after text using NEW-STRING mode. */
        t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, NULL, PPE_STR_OPT_RIGHT_END);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_NOT_EQUAL(t, b);
        CU_ASSERT_EQUAL(ppe_cs_size(t), 29);
        CU_ASSERT_TRUE(ppe_cs_equal_to(t, " \t Text surrounded by spaces."));
        ppe_cs_destroy(t);

        /* Case-4-7: Trim all blank characters surrounding text using MEASURE mode. */
        bsz = sizeof(b);
        t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, &bsz, PPE_STR_OPT_BOTH_ENDS);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, 26);

        /* Case-4-8: Trim all blank characters surrounding text using FILL-BUFFER mode. */
        bsz = sizeof(b);
        memset(b, 0, sizeof(b));
        t = ppe_cs_trim(s, PPE_STR_SPACES, b, &bsz, PPE_STR_OPT_BOTH_ENDS);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, 26);
        CU_ASSERT_EQUAL(memcmp(t, "Text surrounded by spaces.", bsz), 0);

        /* Case-4-9: Trim all blank characters surrounding using NEW-STRING mode. */
        t = ppe_cs_trim(s, PPE_STR_SPACES, NULL, NULL, PPE_STR_OPT_BOTH_ENDS);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_NOT_EQUAL(t, b);
        CU_ASSERT_EQUAL(ppe_cs_size(t), 26);
        CU_ASSERT_TRUE(ppe_cs_equal_to(t, "Text surrounded by spaces."));
        ppe_cs_destroy(t);
    }
} /* test_cs_trim */

static void test_cs_chop(void)
{
    ppe_cstr_c s = NULL;
    ppe_cstr_c t = NULL;
    ppe_size bsz = 0;
    ppe_char b[32];

    /* Case-1: Chop empty string. */
    {
        s = "";

        /* Case-1-1: Chop empty string using MEASURE mode. */
        bsz = sizeof(b);
        t = ppe_cs_chop(s, NULL, &bsz, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, 0);

        /* Case-1-2: Chop empty string using FILL-BUFFER mode. */
        bsz = sizeof(b);
        memset(b, 0, sizeof(b));
        t = ppe_cs_chop(s, b, &bsz, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, 0);

        /* Case-1-3: Chop empty string using NEW-STRING mode. */
        t = ppe_cs_chop(s, NULL, NULL, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_NOT_EQUAL(t, b);
        CU_ASSERT_PTR_EQUAL(t, ppe_cs_empty());
        ppe_cs_destroy(t);
    }

    /* Case-2: Chop string which contains only one byte. */
    {
        s = "A";

        /* Case-2-1: Chop string using MEASURE mode. */
        bsz = sizeof(b);
        t = ppe_cs_chop(s, NULL, &bsz, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, 0);

        /* Case-2-2: Chop string using FILL-BUFFER mode. */
        bsz = sizeof(b);
        memset(b, 0, sizeof(b));
        t = ppe_cs_chop(s, b, &bsz, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, 0);

        /* Case-2-3: Chop string using NEW-STRING mode. */
        t = ppe_cs_chop(s, NULL, NULL, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_NOT_EQUAL(t, b);
        CU_ASSERT_PTR_EQUAL(t, ppe_cs_empty());
        ppe_cs_destroy(t);
    }

    /* Case-3: Chop string which contains two bytes. */
    {
        s = "AB";

        /* Case-3-1: Chop string using MEASURE mode. */
        bsz = sizeof(b);
        t = ppe_cs_chop(s, NULL, &bsz, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, 1);

        /* Case-3-2: Chop string using FILL-BUFFER mode. */
        bsz = sizeof(b);
        memset(b, 0, sizeof(b));
        t = ppe_cs_chop(s, b, &bsz, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, 1);
        CU_ASSERT_EQUAL(memcmp(b, "A", 1), 0);

        /* Case-3-3: Chop string using NEW-STRING mode. */
        t = ppe_cs_chop(s, NULL, NULL, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_NOT_EQUAL(t, b);
        CU_ASSERT_PTR_EQUAL(ppe_cs_size(t), 1);
        CU_ASSERT_TRUE(ppe_cs_equal_to(t, "A"));
        ppe_cs_destroy(t);
    }
} /* test_cs_chop */

static void test_cs_chomp(void)
{
    ppe_cstr_c s = NULL;
    ppe_cstr_c rs = NULL;
    ppe_cstr_c t = NULL;
    ppe_size bsz = 0;
    ppe_char b[32];

    /* Case-1: Chomp empty string. */
    {
        s = "";
        rs = PPE_STR_NEWLINE;

        /* Case-1-1: Chomp empty string using MEASURE mode. */
        bsz = sizeof(b);
        t = ppe_cs_chomp(s, rs, 1, NULL, &bsz, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, 0);

        /* Case-1-2: Chomp empty string using FILL-BUFFER mode. */
        bsz = sizeof(b);
        memset(b, 0, sizeof(b));
        t = ppe_cs_chomp(s, rs, 1, b, &bsz, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, 0);

        /* Case-1-3: Chomp empty string using NEW-STRING mode. */
        t = ppe_cs_chomp(s, rs, 1, NULL, NULL, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_NOT_EQUAL(t, b);
        CU_ASSERT_PTR_EQUAL(t, ppe_cs_empty());
        ppe_cs_destroy(t);
    }

    /* Case-2: Chomp blank string. */
    {
        s = PPE_STR_NEWLINE;
        rs = PPE_STR_NEWLINE;

        /* Case-2-1: Chomp blank string using MEASURE mode. */
        bsz = sizeof(b);
        t = ppe_cs_chomp(s, rs, 1, NULL, &bsz, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, 0);

        /* Case-2-2: Chomp blank string using FILL-BUFFER mode. */
        bsz = sizeof(b);
        memset(b, 0, sizeof(b));
        t = ppe_cs_chomp(s, rs, 1, b, &bsz, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, 0);

        /* Case-2-3: Chomp blank string using NEW-STRING mode. */
        t = ppe_cs_chomp(s, rs, 1, NULL, NULL, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_NOT_EQUAL(t, b);
        CU_ASSERT_PTR_EQUAL(t, ppe_cs_empty());
        ppe_cs_destroy(t);

        /* Case-2-4: Chomp blank strings using MEASURE mode. */
        s = PPE_STR_NEWLINE PPE_STR_NEWLINE;
        bsz = sizeof(b);
        t = ppe_cs_chomp(s, rs, 1, NULL, &bsz, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, ppe_cs_size(PPE_STR_NEWLINE));

        /* Case-2-5: Chomp blank strings using FILL-BUFFER mode. */
        bsz = sizeof(b);
        memset(b, 0, sizeof(b));
        t = ppe_cs_chomp(s, rs, 1, b, &bsz, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, ppe_cs_size(PPE_STR_NEWLINE));
        CU_ASSERT_EQUAL(memcmp(b, PPE_STR_NEWLINE, bsz), 0);

        /* Case-2-6: Chomp blank strings using NEW-STRING mode. */
        t = ppe_cs_chomp(s, rs, 1, NULL, NULL, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_NOT_EQUAL(t, b);
        CU_ASSERT_TRUE(ppe_cs_equal_to(t, PPE_STR_NEWLINE));
        ppe_cs_destroy(t);

        /* Case-2-7: Chomp blank strings using MEASURE mode. */
        s = PPE_STR_NEWLINE PPE_STR_NEWLINE;
        bsz = sizeof(b);
        t = ppe_cs_chomp(s, rs, 2, NULL, &bsz, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, 0);

        /* Case-2-8: Chomp blank strings using FILL-BUFFER mode. */
        bsz = sizeof(b);
        memset(b, 0, sizeof(b));
        t = ppe_cs_chomp(s, rs, 2, b, &bsz, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, 0);

        /* Case-2-9: Chomp blank strings using NEW-STRING mode. */
        t = ppe_cs_chomp(s, rs, 2, NULL, NULL, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_NOT_EQUAL(t, b);
        CU_ASSERT_PTR_EQUAL(t, ppe_cs_empty());
        ppe_cs_destroy(t);

        /* Case-2-10: Chomp blank strings using MEASURE mode. */
        s = PPE_STR_NEWLINE PPE_STR_NEWLINE;
        bsz = sizeof(b);
        t = ppe_cs_chomp(s, rs, -1, NULL, &bsz, 0);
        CU_ASSERT_PTR_NULL(t);
        CU_ASSERT_EQUAL(ppe_err_get_code(), PPE_ERR_TRY_AGAIN);
        CU_ASSERT_EQUAL(bsz, 0);

        /* Case-2-11: Chomp blank strings using FILL-BUFFER mode. */
        bsz = sizeof(b);
        memset(b, 0, sizeof(b));
        t = ppe_cs_chomp(s, rs, -1, b, &bsz, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_EQUAL(t, b);
        CU_ASSERT_EQUAL(bsz, 0);

        /* Case-2-12: Chomp blank strings using NEW-STRING mode. */
        t = ppe_cs_chomp(s, rs, -1, NULL, NULL, 0);
        CU_ASSERT_PTR_NOT_NULL(t);
        CU_ASSERT_PTR_NOT_EQUAL(t, s);
        CU_ASSERT_PTR_NOT_EQUAL(t, b);
        CU_ASSERT_PTR_EQUAL(t, ppe_cs_empty());
        ppe_cs_destroy(t);
    }
} /* test_cs_chomp */

CU_TestInfo test_normal_cases[] = {
    {"test_cs_empty()", test_cs_empty},
    {"test_cs_size()", test_cs_size},
    {"test_cs_is_empty()", test_cs_is_empty},
    {"test_cs_compare()", test_cs_compare},
    {"test_cs_find()", test_cs_find},
    {"test_cs_create()", test_cs_create},
    {"test_cs_slice()", test_cs_slice},
    {"test_cs_trim()", test_cs_trim},
    {"test_cs_chop()", test_cs_chop},
    {"test_cs_chomp()", test_cs_chomp},
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

    pSuite = CU_add_suite("Suite_Test_String", NULL, NULL);
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
