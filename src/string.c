#include "ppe/mempool.h"
#include "ppe/string.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ==== Declaration : String Common ==== */

/* ==== Declaration : String ==== */

/* ---- Types ---- */

typedef struct PPE_STRING
{
    ppe_size len;
    char buf[1];
} ppe_string_st;

/* ---- Functions ---- */

/* -- Internals -- */

static inline ppe_size ppe_cs_detect_size(const char * restrict src, const ppe_size len)
{
    return (len == PPE_STR_DETECT_LENGTH) ? strlen(src) : len;
}

static inline ppe_bool ppe_cs_subtract_remainder(ppe_size * restrict rem, const ppe_size len)
{
    if ((len & ~PPE_STR_MAX_LENGTH) == 0 && *rem >= len) {
        *rem -= len;
        return ppe_true;
    }
    return ppe_false;
}

static inline ppe_bool ppe_str_is_valid(void * restrict str)
{
    return (str && str != PPE_STR_ARG_END);
}

/* ==== Definitions : String ==== */

/* ---- Preset values ---- */

static ppe_string_st str_empty_s = { 0, {""} };
static ppe_string str_empty_array_s[2] = { &str_empty_s, NULL };

/* ---- Functions ---- */

/* -- Internals -- */

/* split */

PPE_API ppe_string * ppe_cs_split(const char * restrict d, const ppe_size dsz, const ppe_string restrict s, ppe_uint * restrict n)
{
}

PPE_API extern ppe_string * ppe_cs_split_cstr(const char * restrict d, const ppe_size dsz, const char * restrict s, const ppe_size sz, ppe_uint * restrict n)
{
}

/* join */

static ppe_string ppe_cs_join_2_imp(const char * restrict deli, ppe_size deli_len, const char * restrict s1, const ppe_size sz1, const char * restrict s2, const ppe_size sz2)
{
    ppe_string new_str = NULL;
    ppe_size remainder = PPE_STR_MAX_LENGTH;

    if (sz1 > 0 && ! ppe_cs_subtract_remainder(&remainder, sz1)) {
        ppe_err_set(PPE_ERR_OVERFLOW_UPPER_BOUND, NULL);
        return NULL;
    }
    if (sz2 > 0 && ! ppe_cs_subtract_remainder(&remainder, sz2)) {
        ppe_err_set(PPE_ERR_OVERFLOW_UPPER_BOUND, NULL);
        return NULL;
    }

    if (remainder == PPE_STR_MAX_LENGTH) return (deli_len > 0) ? ppe_cs_clone(deli, deli_len) : &str_empty_s;

    if (deli_len > 0 && ! ppe_cs_subtract_remainder(&remainder, deli_len)) {
        ppe_err_set(PPE_ERR_OVERFLOW_UPPER_BOUND, NULL);
        return NULL;
    }

    new_str = (ppe_string) ppe_mp_malloc(sizeof(ppe_string_st) + PPE_STR_MAX_LENGTH - remainder);
    if (! new_str) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }
    new_str->len = 0;

    if (sz1 > 0) {
        memcpy(new_str->buf + new_str->len, s1, sz1);
        new_str->len += sz1;
    }
    if (deli_len > 0) {
        memcpy(new_str->buf + new_str->len, deli, deli_len);
        new_str->len += deli_len;
    }
    if (sz2 > 0) {
        memcpy(new_str->buf + new_str->len, s2, sz2);
        new_str->len += sz2;
    }

    new_str->buf[new_str->len] = '\0';
    return new_str;
}

static ppe_string ppe_cs_join_text_list_imp(const char * restrict deli, const ppe_size deli_len, ppe_text_st * restrict srcs, const ppe_uint n)
{
    ppe_uint i = 0;
    ppe_uint m = 0;
    ppe_size deli_len = 0;
    ppe_size remainder = 0;

    if (deli_len > 0) {
        if (srcs[0].len > 0 && ! ppe_cs_subtract_remainder(&remainder, srcs[0].len)) {
            ppe_err_set(PPE_ERR_OVERFLOW_UPPER_BOUND, NULL);
            return NULL;
        }
        for (i = 1; i < n; i += 1) {
            if (! ppe_cs_subtract_remainder(&remainder, deli_len)) {
                ppe_err_set(PPE_ERR_OVERFLOW_UPPER_BOUND, NULL);
                return NULL;
            }

            if (srcs[i].len > 0 && ! ppe_cs_subtract_remainder(&remainder, srcs[i].len)) {
                ppe_err_set(PPE_ERR_OVERFLOW_UPPER_BOUND, NULL);
                return NULL;
            }
        } /* for */
    } else {
        for (i = 0; i < n; i += 1) {
            if (srcs[i].len == 0) continue;
            if (! ppe_cs_subtract_remainder(&remainder, srcs[i].len)) {
                ppe_err_set(PPE_ERR_OVERFLOW_UPPER_BOUND, NULL);
                return NULL;
            }
            srcs[m].buf = srcs[i].buf;
            srcs[m].len = srcs[i].len;
            m += 1;
        } /* for */

        /* Concatenate empty strings. */
        if (m == 0) return &str_empty_s;
        if (m == 1) return ppe_cs_clone(srcs[0].buf, srcs[0].len);
    }

    new_str = (ppe_string) ppe_mp_malloc(sizeof(ppe_string_st) + PPE_STR_MAX_LENGTH - remainder);
    if (! new_str) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }
    new_str->len = 0;

    if (deli_len > 0) {
        memcpy(new_str->buf + new_str->len, srcs[0].buf, srcs[0].len);
        new_str->len += srcs[0].len;

        for (i = 1; i < n; i += 1) {
            memcpy(new_str->buf + new_str->len, deli, deli_len);
            new_str->len += deli_len;

            if (srcs[i].len == 0) continue;
            memcpy(new_str->buf + new_str->len, srcs[i].buf, srcs[i].len);
            new_str->len += srcs[i].len;
        } /* for */
    } else {
        for (i = 0; i < m; i += 1) {
            memcpy(new_str->buf + new_str->len, srcs[i].buf, srcs[i].len);
            new_str->len += srcs[i].len;
        } /* for */
    }

    new_str[new_str->len] = '\0';
    return new_str;
}

/* -- Preset values -- */

PPE_API const ppe_string ppe_str_get_empty(void)
{
    return &str_empty_s;
}

/* -- Create & Destroy -- */

/* _cs_ series */

PPE_API ppe_string ppe_cs_clone(const ppe_char * restrict s, const ppe_size sz)
{
    ppe_string nw = NULL;

    if (! ppe_str_valid_string(s)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }

    if (s == str_empty_s.buf) {
        return &str_empty_s;
    }
    if (sz == PPE_STR_DETECT_LENGTH) {
        sz = strlen(s);
    }

    if (sz == 0) {
        /* Check if the source string is empty. */
        return &str_empty_s;
    }

    nw = (ppe_string) ppe_mp_malloc(sizeof(ppe_string_st) + sz);
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    memcpy(nw->buf, s, sz);
    nw->buf[sz] = '\0';
    nw->sz = sz;
    return nw;
}

PPE_API ppe_string ppe_cs_join_cstr_var(const char * restrict deli, const ppe_size deli_len, const char * restrict s1, const char * restrict s2, va_list args)
{
    va_list ap;
    const char * src = NULL;
    ppe_uint n = 0;
    ppe_text_st tmp_srcs[PPE_STR_MAX_INPUT_N];

    if (! ppe_str_is_valid(deli) || ! ppe_str_is_valid(s1) || ! ppe_str_is_valid(s2)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }

    /* -- Check arguments. -- */
    va_copy(ap, args);

    src = va_arg(ap, const char *);
    if (! src) {
        va_end(ap);
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (src == PPE_STR_ARG_END) {
        /* Only two strings to join. */
        va_end(ap);
        return ppe_cs_join_2_imp(deli, deli_len, s1, strlen(s1), s2, strlen(s2));
    }

    /* -- Prepare operands. -- */
    tmp_srcs[0].buf = s1;
    tmp_srcs[0].len = strlen(s1);
    tmp_srcs[1].buf = s2;
    tmp_srcs[1].len = strlen(s2);
    n = 2;

    do {
        if (n < PPE_STR_MAX_INPUT_N) {
            tmp_srcs[n].buf = src;
            tmp_srcs[n].len = strlen(src);
            n += 1;
        } else {
            va_end(ap);
            ppe_err_set(PPE_ERR_OUT_OF_RANGE, NULL);
            return NULL;
        }

        src = va_arg(ap, const char *);
        if (! src) {
            va_end(ap);
            ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
            return NULL;
        }
    } while (src != PPE_STR_ARG_END);

    va_end(ap);

    return ppe_cs_join_text_list_imp(deli, deli_len, tmp_srcs, n);
}

PPE_API ppe_string ppe_cs_join_cstr_list(const char * restrict deli, const ppe_size deli_len, const char ** restrict srcs, const ppe_uint n)
{
    ppe_uint i = 0;
    ppe_text_st tmp_srcs[PPE_STR_MAX_INPUT_N];

    if (! ppe_str_is_valid(deli) || ! srcs || n > PPE_STR_MAX_INPUT_N) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    
    if (n == 0) return &str_empty_s;
    if (n == 1) return ppe_cs_clone(srcs[0], PPE_STR_DETECT_LENGTH);

    for (i = 0; i < n; i += 1) {
        if (! srcs[i] || srcs[i] == PPE_STR_ARG_END) {
            ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
            return NULL;
        }

        tmp_srcs[i].buf = srcs[i];
        tmp_srcs[i].len = strlen(srcs[i]);
    }

    return ppe_cs_join_text_list_imp(deli, deli_len, tmp_srcs, n);
}

PPE_API ppe_string ppe_cs_join_text_var(const char * restrict deli, const ppe_size deli_len, const char * restrict s1, const ppe_size sz1, const char * restrict s2, const ppe_size sz2, va_list args)
{
    va_list ap;
    const char * src = NULL;
    ppe_size len = 0;
    ppe_uint n = 0;
    ppe_text_st tmp_srcs[PPE_STR_MAX_INPUT_N];

    if (! ppe_str_is_valid(deli) || ! ppe_str_is_valid(s1) || ! ppe_str_is_valid(s2)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }

    /* -- Check arguments. -- */
    va_copy(ap, args);

    src = va_arg(ap, const char *);
    if (! src) {
        va_end(ap);
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (src == PPE_STR_ARG_END) {
        /* Only two strings to join. */
        va_end(ap);
        return ppe_cs_join_2_imp(deli, deli_len, s1, ppe_cs_detect_size(s1, sz1), s2, ppe_cs_detect_size(s2, sz2));
    }

    /* -- Prepare operands. -- */
    tmp_srcs[0].buf = s1;
    tmp_srcs[0].len = ppe_cs_detect_size(s1, sz1);
    tmp_srcs[1].buf = s2;
    tmp_srcs[1].len = ppe_cs_detect_size(s2, sz2);
    n = 2;

    do {
        if (n < PPE_STR_MAX_INPUT_N) {
            len = va_arg(ap, ppe_size);
            tmp_srcs[n].buf = src;
            tmp_srcs[n].len = ppe_cs_detect_size(src, len);
            n += 1;
        } else {
            va_end(ap);
            ppe_err_set(PPE_ERR_OUT_OF_RANGE, NULL);
            return NULL;
        }

        src = va_arg(ap, const char *);
        if (! src) {
            va_end(ap);
            ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
            return NULL;
        }
    } while (src != PPE_STR_ARG_END);

    va_end(ap);

    return ppe_cs_join_text_list_imp(deli, deli_len, tmp_srcs, n);
}

PPE_API ppe_string ppe_cs_join_text_list(const char * restrict deli, const ppe_size deli_len, const ppe_text_st * restrict srcs, const ppe_uint n)
{
    ppe_uint i = 0;
    ppe_text_st tmp_srcs[PPE_STR_MAX_INPUT_N];

    if (! ppe_str_is_valid(deli) || ! srcs || n > PPE_STR_MAX_INPUT_N) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }

    if (n == 0) return &str_empty_s;
    if (n == 1) return (srcs[0].len > 0) ? ppe_cs_clone(srcs[0].buf, srcs[0].len) : &str_empty_s;
    
    for (i = 0; i < n; i += 1) {
        tmp_srcs[i].buf = srcs[i].buf;
        tmp_srcs[i].len = ppe_cs_detect_size(srcs[i].buf, srcs[i].len);
    }
    return ppe_cs_join_text_list_imp(deli, deli_len, tmp_srcs, n);
}

PPE_API ppe_string ppe_cs_join_var(const char * restrict deli, const ppe_size deli_len, const ppe_string restrict s1, const ppe_string restrict s2, va_list args)
{
    va_list ap;
    const ppe_string src = NULL;
    ppe_uint n = 0;
    ppe_text_st tmp_srcs[PPE_STR_MAX_INPUT_N];

    if (! ppe_str_is_valid(deli) || ! ppe_str_is_valid(s1) || ! ppe_str_is_valid(s2)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }

    /* -- Check arguments -- */
    va_copy(ap, args);

    src = va_arg(ap, const ppe_string);
    if (! src) {
        va_end(ap);
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (src == PPE_STR_ARG_END) {
        /* Only two strings to join. */
        va_end(ap);
        return ppe_cs_join_2_imp(deli, deli_len, s1->buf, s1->len, s2->buf, s2->len);
    }

    /* -- Prepare operands. -- */
    tmp_srcs[0].buf = s1->buf;
    tmp_srcs[0].len = s1->len;
    tmp_srcs[1].buf = s2->buf;
    tmp_srcs[1].len = s2->len;
    n = 2;

    do {
        if (n < PPE_STR_MAX_INPUT_N) {
            tmp_srcs[n].buf = src->buf;
            tmp_srcs[n].len = src->len;
            n += 1;
        } else {
            va_end(ap);
            ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
            return NULL;
        }

        src = va_arg(ap, const ppe_string);
        if (! src) {
            va_end(ap);
            ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
            return NULL;
        }
    } while (src != PPE_STR_ARG_END);

    va_end(ap);

    return ppe_cs_join_text_list_imp(deli, deli_len, tmp_srcs, n);
}

PPE_API ppe_string ppe_cs_join_list(const char * restrict deli, const ppe_size deli_len, const ppe_string * restrict srcs, const ppe_uint n)
{
    ppe_uint i = 0;
    ppe_text_st tmp_srcs[PPE_STR_MAX_INPUT_N];

    if (! ppe_str_is_valid(deli) || ! srcs || n > PPE_STR_MAX_INPUT_N) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }

    if (n == 0) return &str_empty_s;
    if (n == 1) return (srcs[i]->len > 0) ? ppe_cs_clone(srcs[i]->buf, srcs[i]->len) : &str_empty_s;

    for (i = 0; i < n; i += 1) {
        tmp_srcs[i].buf = srcs[i]->buf;
        tmp_srcs[i].len = srcs[i]->len;
    }
    return ppe_cs_join_text_list_imp(deli, deli_len, tmp_srcs, n);
}

PPE_API ppe_string ppe_cs_join_2(const char * restrict deli, const ppe_size deli_len, const char * restrict s1, const ppe_size sz1, const char * restrict s2, const ppe_size sz2)
{
    if (! ppe_str_is_valid(deli) || ! ppe_str_is_valid(s1) || ! ppe_str_is_valid(s2)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    sz1 = ppe_cs_detect_size(s1, sz1);
    sz2 = ppe_cs_detect_size(s2, sz2);
    return ppe_cs_join_2_imp(deli, deli_len, s1, sz1, s2, sz2);
}

PPE_API ppe_string ppe_cs_vsprintf(const char * restrict fmt, va_list args)
{
    /* TODO: Implementation */
    return NULL;
}

/* _str_ series */

PPE_API ppe_string ppe_str_clone(const ppe_string restrict src)
{
    return ppe_cs_clone(src->buf, src->len);
}

/* split */

PPE_API ppe_string * ppe_str_split_cstr(const ppe_string restrict d, const char * restrict s, const ppe_size sz, ppe_uint * restrict n)
{
    ppe_str_tracer trc = NULL;
    ppe_string * arr = NULL;
    ppe_string * arr2 = NULL;
    ppe_string * tmp = NULL;
    ppe_int ret = 0;
    ppe_uint cnt = 0;
    ppe_uint cnt2 = 0;

    if (! ppe_str_is_valid(d) || ! ppe_str_is_valid(s)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }

    if (sz == 0) {
        /* Try to split an empty string. */
        return &str_empty_array_s;
    }

    if (! (trc = ppe_stc_create_for_cstr(s, sz))) {
        return NULL;
    }

    /* NOTICE: *n may be greater than the entry number limit (64) of the string tracer. */
    ret = ppe_stc_find_cstr(trc, d->buf, d->len, *n);
    if (ret < 0) {
        ppe_stc_destroy(trc);
        return NULL;
    }
    cnt = (ppe_uint) ret;

    arr = ppe_stc_split(trc, 0, &cnt);
    if (! arr) {
        ppe_stc_destroy(trc);
        return NULL;
    }

    ppe_stc_continue(trc);

    while ((ret = ppe_stc_find_cstr(trc, d->buf, d->len, *n - cnt)) > 0) {
        cnt2 = (ppe_uint) ret;

        arr2 = ppe_stc_split(trc, 0, &cnt2);
        if (! arr2) {
            goto PPE_STR_SPLIT_CSTR_ERROR_HANDLING;
        }

        tmp = ppe_mp_malloc((cnt + cnt2 + 1) * sizeof(ppe_string));
        if (! tmp) {
            ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
            goto PPE_STR_SPLIT_CSTR_ERROR_HANDLING;
        }

        memcpy(tmp, arr, cnt * sizeof(ppe_string));
        memcpy(tmp + cnt * sizeof(ppe_string), arr2, cnt2 * sizeof(ppe_string));

        ppe_mp_free(arr2);
        ppe_mp_free(arr);
        arr = tmp;

        cnt += cnt2;

        ppe_stc_continue(trc);
    } /* while */

    ppe_stc_destroy(trc);
    arr[cnt] = NULL;
    *n = cnt;
    return arr;

PPE_STR_SPLIT_CSTR_ERROR_HANDLING:
    while (cnt > 0) {
        ppe_str_destroy(arr[--cnt]);
    }
    ppe_mp_free(arr);
    return NULL;
}

PPE_API ppe_string * ppe_str_split(const ppe_string restrict d, const ppe_string restrict s, ppe_uint * restrict n)
{
    return ppe_str_split_cstr(d, s->buf, s->len, n);
}

/* join */

PPE_API ppe_string ppe_str_join_cstr_var(const ppe_string restrict deli, const char * restrict s1, const char * restrict s2, va_list args)
{
    if (! ppe_str_is_valid(deli)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    return ppe_cs_join_cstr_var(deli->buf, deli->len, s1, s2, args);
}

PPE_API ppe_string ppe_str_join_cstr_list(const ppe_string restrict deli, const char ** restrict srcs, const ppe_uint n)
{
    if (! ppe_str_is_valid(deli)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    return ppe_cs_join_cstr_list(deli->buf, deli->len, srcs, n);
}

PPE_API ppe_string ppe_str_join_text_var(const ppe_string restrict deli, const char * restrict s1, const ppe_size sz1, const char * restrict s2, const ppe_size sz2, va_list args)
{
    if (! ppe_str_is_valid(deli)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    return ppe_cs_join_text_var(deli->buf, deli->len, s1, sz1, s2, sz2, args);
}

PPE_API ppe_string ppe_str_join_text_list(const ppe_string restrict deli, const ppe_text_st * restrict srcs, const ppe_uint n)
{
    if (! ppe_str_is_valid(deli)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    return ppe_cs_join_cstr_list(deli->buf, deli->len, srcs, n);
}

PPE_API ppe_string ppe_str_join_var(const ppe_string restrict deli, const ppe_string restrict s1, const ppe_string restrict s2, va_list args)
{
    if (! ppe_str_is_valid(deli)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    return ppe_cs_join_var(deli->buf, deli->len, s1, s2, args);
}

PPE_API ppe_string ppe_str_join_list(const ppe_string restrict deli, const ppe_string * restrict srcs, const ppe_uint n)
{
    if (! ppe_str_is_valid(deli)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    return ppe_cs_join_list(deli->buf, deli->len, srcs, n);
}

PPE_API ppe_string ppe_str_join_2(const ppe_string restrict deli, const ppe_string restrict s1, const ppe_string restrict s2)
{
    if (! ppe_str_is_valid(deli) || ! ppe_str_is_valid(s1) || ! ppe_str_is_valid(s2)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    return ppe_cs_join_2_imp(deli->buf, deli->len, s1->buf, s1->len, s2->buf, s2->len);
}

PPE_API void ppe_str_destroy(ppe_string restrict str)
{
    if (str && str != &str_empty_s) ppe_mp_free(str);
}

/* -- Property -- */

PPE_API const char * ppe_str_cstr(ppe_string restrict str)
{
    if (! ppe_str_is_valid(str)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    return str->buf;
}

PPE_API ppe_size ppe_str_size(ppe_string restrict str)
{
    if (! ppe_str_is_valid(str)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return PPE_STR_INVALID_LENGTH;
    }
    return str->len;
}

/* -- Comparison -- */

PPE_API ppe_bool ppe_str_equals(const ppe_string restrict str1, const ppe_string restrict str2)
{
    if (! ppe_str_is_valid(str1) || ! ppe_str_is_valid(str2)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return PPE_STR_INVALID_LENGTH;
    }
    return strcmp(((ppe_string)str1)->buf, ((ppe_string)str2)->buf) == 0;
}

PPE_API ppe_bool ppe_str_less_than(const ppe_string restrict str1, const ppe_string restrict str2)
{
    if (! ppe_str_is_valid(str1) || ! ppe_str_is_valid(str2)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return PPE_STR_INVALID_LENGTH;
    }
    return strcmp(((ppe_string)str1)->buf, ((ppe_string)str2)->buf) < 0;
}

PPE_API ppe_bool ppe_str_greater_than(const ppe_string restrict str1, const ppe_string restrict str2)
{
    if (! ppe_str_is_valid(str1) || ! ppe_str_is_valid(str2)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return PPE_STR_INVALID_LENGTH;
    }
    return strcmp(((ppe_string)str1)->buf, ((ppe_string)str2)->buf) > 0;
}

/* ==== Declaration : String Tracer ==== */

/* ---- Types ---- */

typedef struct PPE_STR_TRACER
{
    const char * pos; /* The start point of the next search. */

    ppe_text_st s; /* The target string to search in. */

    struct {
        ppe_uint cnt; /* The count of substring matches. */
        ppe_text_st mtc[64]; /* Substring matches. */
    } d;
} ppe_str_tracer_st;

/* ---- Functions ---- */

/* -- Create & Destroy -- */

PPE_API ppe_str_tracer ppe_stc_create(const ppe_string restrict s)
{
    ppe_tracer new_trc = NULL;

    if (! ppe_str_is_valid(s)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }

    new_trc = ppe_mp_calloc(1, sizeof(ppe_tracer_st));
    if (! new_trc) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    new_trc->s.buf = s->buf;
    new_trc->s.len = s->len;

    new_trc->pos = new_trc->s.buf;
    return new_trc;
}

PPE_API ppe_str_tracer ppe_stc_create_for_cstr(const char * restrict s, const ppe_size sz)
{
    ppe_tracer new_trc = NULL;

    if (! ppe_str_is_valid(s)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }

    new_trc = ppe_mp_calloc(1, sizeof(ppe_tracer_st));
    if (! new_trc) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    new_trc->s.buf = s;
    new_trc->s.len = sz;

    new_trc->pos = new_trc->s.buf;
    return new_trc;
}

PPE_API ppe_bool ppe_stc_reset(ppe_str_tracer restrict trc, const ppe_string restrict s)
{
    if (! trc || ! ppe_str_is_valid(s)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return ppe_false;
    }

    memset(&trc, 0, sizeof(trc));

    trc->s.buf = s->buf;
    trc->s.len = s->len;

    trc->pos = trc->s.buf;
    return ppe_true;
}

PPE_API ppe_bool ppe_stc_reset_for_cstr(ppe_str_tracer restrict trc, const char * restrict s, const ppe_size sz)
{
    if (! trc || ! ppe_str_is_valid(s)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return ppe_false;
    }

    memset(&trc->d, 0, sizeof(trc->d));

    trc->s.buf = s;
    trc->s.len = sz;

    trc->pos = trc->s.buf;
    return ppe_true;
}

PPE_API void ppe_stc_destroy(ppe_str_tracer restrict trc)
{
    if (trc) ppe_mp_free(trc);
}

/* -- Finding -- */
static ppe_bool ppe_stc_find_imp(ppe_str_tracer restrict trc, const char * restrict s, const ppe_size sz)
{
    const char * p = NULL;
    const char * e = NULL;
    ppe_size r = 0;

    assert(trc->pos > trc->s.buf);

    r = trc->s.len - (trc->pos - trc->s.buf);
    if (r < sz) {
        /* The part to search of the string is shorter than the target substring. */
        return ppe_false;
    }

    /* If the search failed, don't consume any characters and leave them for the next search of other substrings. */

    if (sz == 1) {
        if ((p = memchr(trc->poc, s[0], r))) {
            trc->d.mtc[trc->d.cnt].buf = p;
            trc->d.mtc[trc->d.cnt].len = sz;
            trc->d.cnt += 1;

            trc->pos = p + sz; /* Stop at the start point of the next search. */
            return ppe_true;
        }

        return ppe_false;
    }

    p = trc->pos;
    e = trc->pos + r;
    while (p < e) {
        p = memchr(p, s[0], r);
        if (! p || memcmp(p, s, sz) != 0) {
            p++;
            continue;
        }

        trc->d.mtc[trc->d.cnt].buf = p;
        trc->d.mtc[trc->d.cnt].len = sz;
        trc->d.cnt += 1;

        trc->pos = p + sz; /* Stop at the start point of the next search. */
        return ppe_true;
    } /* while */

    return ppe_false;
}

PPE_API ppe_int ppe_stc_find_cstr(ppe_str_tracer restrict trc, const char * restrict s, const ppe_size sz, const ppe_uint n)
{
    ppe_int c = 0;
    ppe_uint i = 0;
    ppe_uint max = 0;

    if (! trc || ! ppe_str_is_valid(s) || sz == 0 || n == 0) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return -1;
    }

    max = (sizeof(trc->d.mtc) / sizeof(trc->d.mtc[0])) - trc->d.cnt;
    if (max == 0) {
        ppe_err_set(PPE_ERR_OUT_OF_BUFFER, NULL);
        return -1;
    }

    if (n < max) max = n;
    for (i = 0; i < max && ppe_stc_find_imp(trc, s, sz); ++i, ++c);
    return c;
}

PPE_API ppe_int ppe_stc_find(ppe_str_tracer restrict trc, const ppe_string restrict s, const ppe_uint n)
{
    if (! ppe_str_is_valid(s)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return -1;
    }
    return ppe_stc_find_cstr(trc, s->buf, s->len, n);
}

PPE_API ppe_bool ppe_stc_continue(ppe_str_tracer restrict trc)
{
    if (! trc) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return ppe_false;
    }

    memset(&trc->d, 0, sizeof(trc->d));
    return ppe_true;
}

/* -- Split -- */

static void ppe_stc_locate_section(ppe_str_tracer restrict trc, const ppe_int idx, const char ** restrict s, ppe_size * restrict sz)
{
    if (idx == 0) {
        *s = trc->s.buf;
        *sz = trc->d.mtc[0].buf - trc->s.buf;
    } else if (idx == trc->d.cnt) {
        *s = trc->d.mtc[idx - 1].buf + trc->d.mtc[idx - 1].len;
        *sz = trc->len - (trc->d.mtc[idx - 1].buf - trc->s.buf) - trc->d.mtc[idx - 1].len;
    } else {
        *s = trc->d.mtc[idx - 1].buf + trc->d.mtc[idx - 1].len;
        *sz = trc->d.mtc[idx].buf - *s;
    }
}

PPE_API ppe_string ppe_stc_split_one(ppe_str_tracer restrict trc, const ppe_uint idx)
{
    const char * s = NULL;
    ppe_size sz = 0;

    if (! trc) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (trc->d.cnt == 0) {
        if (idx > trc->d.cnt) {
            ppe_err_set(PPE_ERR_NO_SUCH_ENTRY, NULL);
            return NULL;
        }
        return ppe_cs_clone(trc->s.buf, trc->s.len);
    }

    ppe_stc_locate_section(trc, idx, &s, &sz);
    return ppe_cs_clone(s, sz);
}

PPE_API ppe_string * ppe_stc_split(ppe_str_tracer restrict trc, const ppe_int idx, const ppe_uint * restrict n)
{
    ppe_string * arr = NULL;
    const char * s = NULL;
    ppe_size sz = 0;
    ppe_uint i = 0;
    ppe_uint max = 0;

    if (! trc || (n && *n == 0)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (trc->d.cnt == 0 || trc->d.cnt < idx) {
        ppe_err_set(PPE_ERR_NO_SUCH_ENTRY, NULL);
        return NULL;
    }

    max = trc->d.cnt + 1 - idx;

    if (max == 0) {
        /* No substrings found. */
        arr = (ppe_string *) ppe_mp_malloc(2 * sizeof(ppe_string));
        if (! arr) {
            ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
            return NULL;
        }

        arr[0] = ppe_cs_clone(trc->s.buf, trc->s.len);
        if (! arr[0]) {
            return NULL;
        }

        arr[1] = NULL;
        if (n) *n = 1;
        return arr;
    }

    if (n && *n < max) {
        max = *n;
    }

    arr = (ppe_string *) ppe_mp_malloc((max + 1) * sizeof(ppe_string));
    if (! arr) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    for (i = 0; i < max; ++i) {
        ppe_stc_locate_section(trc, idx + i, &s, &sz);
        arr[i] = ppe_cs_clone(s, sz);
        if (! arr[i]) {
            while (i > 0) {
                ppe_str_destroy(arr[--i]);
            }
            ppe_mp_free(arr);
            return NULL;
        }
    }

    if (n) *n = i;
    return arr;
}

/* -- Replace -- */

PPE_API ppe_string ppe_stc_replace_by_joiner(ppe_str_tracer restrict trc, const ppe_uint idx, const ppe_uint n, ppe_str_joiner restrict jnr)
{
    ppe_uint i = 0;
    ppe_uint max = 0;

    if (! trc || ! jnr) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (trc->d.cnt == 0 || trc->d.cnt < idx) {
        ppe_err_set(PPE_ERR_NO_SUCH_ENTRY, NULL);
        return NULL;
    }

    /* Do not reset the joiner to allow the caller pass in some preceding substrings. */

    max = trc->d.cnt - idx;
    if (n < max) max = n;

    for (i = 0; i < max; ++i) {
        if (! ppe_sjn_append_cstr(jnr, trc->d[idx + i].buf, trc->d[idx + i].len)) {
            return NULL;
        }
    }
    return ppe_sjn_join(jnr, ppe_false); /* Do not reset the joiner after replacing. */
}

PPE_API ppe_string ppe_stc_replace_with_cstr(ppe_str_tracer restrict trc, const ppe_uint idx, const ppe_uint n, const char * restrict s, const ppe_size sz)
{
    ppe_string new_s = NULL;
    ppe_str_joiner jnr = NULL;

    if (! s) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }

    if (! (jnr = ppe_sjn_create(s))) {
        return NULL;
    }

    new_s = ppe_stc_replace_by_joiner(trc, idx, n, jnr);
    ppe_sjn_destroy(jnr);
    return new_s;
}

PPE_API ppe_string ppe_stc_replace(ppe_str_tracer restrict trc, const ppe_int uidx, const ppe_int cnt, const ppe_string restrict s)
{
    ppe_string new_s = NULL;
    ppe_str_joiner jnr = NULL;

    if (! s) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }

    if (! (jnr = ppe_sjn_create(s->buf))) {
        return NULL;
    }

    new_s = ppe_stc_replace_by_joiner(trc, idx, n, jnr);
    ppe_sjn_destroy(jnr);
    return new_s;
}

/* -- Remove -- */

PPE_API ppe_string ppe_stc_remove(ppe_str_tracer restrict trc, const ppe_uint idx, const ppe_uint n)
{
    return ppe_stc_replace(trc, idx, n, &str_empty_s);
}

/* -- Property -- */

PPE_API ppe_int ppe_stc_count(ppe_str_tracer restrict trc)
{
    if (! trc) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    return trc->d.cnt;
}

/* ==== Definition : String Bunch ==== */

/* ---- Types ---- */

enum
{
    PPE_SBC_CSTR_REF = 0,   /* A reference to external C-style string. */
    PPE_SBC_CSTR_DAT = 1,   /* A reference to internal C-style string holding in the buffer. */
    PPE_SBC_STR_REF = 2     /* A reference to external string. */
};

typedef union PPE_SBC_ENTRY
{
    ppe_text_st t;
    ppe_string * s;
} ppe_sbc_entry_st, *ppe_sbc_entry;

typedef struct PPE_SBC_BUFFER
{
    ppe_size rem;
    char buf[1];
} ppe_sbc_buffer_st, *ppe_sbc_buffer;

typedef struct PPE_STR_BUNCH
{
    struct {
        ppe_sbc_entry_st * ent;
        ppe_uint cnt;
        ppe_uint cap;           /* The total count of entries. */
    } e;

    struct {
        ppe_uint * type;
        ppe_uint cnt;
        ppe_uint cap;           /* The total count of types. */
    } t;

    struct {
        ppe_sbc_buffer * dat;
        ppe_uint cnt;
        ppe_size cap;           /* The total count of allocated buffers. */
    } b;
} ppe_str_bunch_st;

/* ---- Functions ---- */

PPE_API ppe_str_bunch ppe_sbc_create(void)
{
    ppe_str_bunch bc = ppe_mp_calloc(1, sizeof(ppe_str_bunch_st));
    if (! bc) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }
    bc->b.cap = 4096;
    return bc;
}

PPE_API void ppe_sbc_destroy(ppe_str_bunch restrict bc)
{
    if (bc) {
        while (bc->b.cap > 0) {
            bc->b.cap -= 1;
            ppe_mp_free(bc->b.dat[bc->b.cap]);
        }
        ppe_mp_free(bc->t.type);
        ppe_mp_free(bc->e.ent);
        ppe_mp_free(bc);
    }
}

PPE_API void ppe_sbc_reset(ppe_str_bunch restrict bc)
{
    if (bc) {
        bc->e.cnt = 0;
        bc->t.cnt = 0;
        bc->b.cnt = 0;
    }
}

#define PPE_SBC_TYPE_CONTAINER_SIZE (sizeof(((ppe_sbc_bunch) 0)->t.type[0]))
#define PPE_SBC_TYPE_CONTAINER_BITS (PPE_SBC_TYPE_CONTAINER_SIZE * 8)

#define PPE_SBC_TYPE_BITS 2
#define PPE_SBC_TYPE_COUNT_PER_CONTAINER (PPE_SBC_TYPE_CONTAINER_BITS / PPE_SBC_TYPE_BITS)

static inline void ppe_sbc_set_type(ppe_str_bunch restrict bc, const ppe_uint idx, const ppe_uint type)
{
    ppe_uint p = idx / PPE_SBC_TYPE_COUNT_PER_CONTAINER;
    ppe_uint q = idx % PPE_SBC_TYPE_COUNT_PER_CONTAINER;

    bc->t.type[p] &= ~ (0x3 << (q * PPE_SBC_TYPE_BITS));        /* Clear bits. */
    bc->t.type[p] |= (type & 0x3) << (q * PPE_SBC_TYPE_BITS);   /* Set bits. */
}

static ppe_bool ppe_sbc_augment_entries(ppe_str_bunch restrict bc)
{
    ppe_sbc_entry_st * nw = NULL;
    ppe_uint cap = 0;

    cap = (bc->e.cap > 0) ? (bc->e.cap + (bc->e.cap >> 1)) : 16; /* The new capacity is 1.5 times of the old one. */

    nw = (ppe_sbc_entry_st *) ppe_mp_calloc(cap, sizeof(ppe_sbc_entry_st));
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return ppe_false;
    }

    if (bc->e.cnt > 0) {
        memcpy(nw, bc->e.ent, bc->e.cnt * sizeof(ppe_sbc_entry_st));
        ppe_mp_free(bc->e.ent);
    }

    bc->e.ent = nw;
    bc->e.cap = cap;
    return ppe_true;
}

static inline ppe_uint ppe_sbc_type_container_count(ppe_uint cap)
{
    cap *= 2; /* Turn to the count of bits. */
    if (cap & (ppe_uint)(PPE_SBC_TYPE_CONTAINER_BITS - 1)) {
        cap = (cap + PPE_SBC_TYPE_CONTAINER_BITS) & (~ (ppe_uint)(PPE_SBC_TYPE_CONTAINER_BITS - 1));
    }
    return cap / PPE_SBC_TYPE_CONTAINER_BITS; /* Turn back to the count of type containers. */
}

static ppe_bool ppe_sbc_augment_types(ppe_str_bunch restrict bc)
{
    ppe_uint * nw = NULL;
    ppe_uint cap = 0;

    cap = (bc->t.cap > 0) ? (bc->t.cap + (bc->t.cap >> 1)) : 128; /* The new capacity is 1.5 times of the old one. */

    nw = (ppe_uint *) ppe_mp_calloc(ppe_sbc_type_container_count(cap), PPE_SBC_TYPE_CONTAINER_SIZE);
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return ppe_false;
    }

    if (bc->t.cnt > 0) {
        memcpy(nw, bc->t.type, ppe_sbc_type_container_count(bc->t.cnt) * PPE_SBC_TYPE_CONTAINER_SIZE);
        ppe_mp_free(bc->t.type);
    }

    bc->t.type = nw;
    bc->t.cap = cap;
    return ppe_true;
}

static ppe_bool ppe_sbc_augment_buffers(ppe_str_bunch restrict bc)
{
    ppe_sbc_buffer * nw = NULL;
    ppe_uint cap = 0;

    if (bc->b.cnt == bc->b.cap) {
        cap = (bc->b.cap > 0) ? (bc->b.cap + (bc->b.cap >> 1)) : 8; /* The new capacity is 1.5 times of the old one. */

        nw = (ppe_sbc_buffer *) ppe_mp_calloc(cap, sizeof(ppe_sbc_buffer));
        if (! nw) {
            ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
            return ppe_false;
        }

        if (bc->b.cnt > 0) {
            memcpy(nw, bc->b.dat, bc->b.cnt * sizeof(ppe_sbc_buffer));
            ppe_mp_free(bc->b.dat);
        }

        bc->b.dat = nw;
        bc->b.cap = cap;
    }

    bc->bc.cnt += 1;
    if (! bc->b.dat[bc->b.cnt]) {
        bc->b.dat[bc->b.cnt] = (ppe_sbc_buffer) ppe_mp_malloc(4096);
        if (! bc->b.dat[bc->b.cnt]) {
            ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
            bc->bc.cnt -= 1;
            return ppe_false;
        }
    }
    bc->b.dat[bc->b.cnt]->rem = 4096; /* TODO: Define a buffer size aligning to page size. */
    return ppe_true;
}

PPE_API ppe_bool ppe_sbc_refer_to_cstr(ppe_str_bunch restrict bc, const char * restrict s, const ppe_size sz)
{
    if (! bc || ! ppe_str_is_valid(s)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return ppe_false;
    }

    if (bc->e.cnt == bc->e.cap && ! ppe_sbc_augment_entries(bc)) {
        return ppe_false;
    }
    if (bc->t.cnt == bc->t.cap && ! ppe_sbc_augment_types(bc)) {
        return ppe_false;
    }

    bc->e.ent[bc->e.cnt].t.buf =  s;
    bc->e.ent[bc->e.cnt].t.len =  sz;

    ppe_sbc_set_type(bc, bc->t.cnt, PPE_SBC_CSTR_REF);

    bc->e.cnt += 1;
    bc->t.cnt += 1;
    return ppe_true;
}

PPE_API ppe_bool ppe_sbc_refer_to(ppe_str_bunch restrict bc, const ppe_string restrict s)
{
    if (! bc || ! ppe_str_is_valid(s)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return ppe_false;
    }

    if (bc->e.cnt == bc->e.cap && ! ppe_sbc_augment_entries(bc)) {
        return ppe_false;
    }
    if (bc->t.cnt == bc->t.cap && ! ppe_sbc_augment_types(bc)) {
        return ppe_false;
    }

    /* TODO: Add reference counting. */
    bc->e.ent[bc->e.cnt].s =  s;

    ppe_sbc_set_type(bc, bc->t.cnt, PPE_SBC_STR_REF);

    bc->e.cnt += 1;
    bc->t.cnt += 1;
    return ppe_true;
}

static ppe_bool ppe_sbc_copy_from_imp(ppe_str_bunch restrict bc, const char * restrict s, const ppe_size sz)
{
    if (bc->e.cnt == bc->e.cap && ! ppe_sbc_augment_entries(bc)) {
        return ppe_false;
    }
    if (bc->t.cnt == bc->t.cap && ! ppe_sbc_augment_types(bc)) {
        return ppe_false;
    }
    if (bc->b.cnt == 0 || bc->b.dat[bc->b.cnt - 1]->rem < sz) {
        if (! ppe_sbc_augment_buffers(bc)) {
            return ppe_false;
        }
    }

    ppe_sbc_set_type(bc, bc->t.cnt, PPE_SBC_CSTR_DAT);

    /* TODO: Align the start address to which copy the string. */
    bc->e.ent[bc->e.cnt].t.buf = bc->b.dat[bc->b.cnt - 1]->buf + (4096 - bc->b.dat[bc->b.cnt - 1]->rem);
    bc->e.ent[bc->e.cnt].t.len = sz;
    memcpy(bc->e.ent[bc->e.cnt].buf, s, sz);

    bc->e.cnt += 1;
    bc->t.cnt += 1;
    bc->b.dat[bc->b.cnt - 1]->rem -= sz;
    return ppe_true;
}

PPE_API ppe_bool ppe_sbc_copy_from_cstr(ppe_str_bunch restrict bc, const char * restrict s, const ppe_size sz)
{
    if (! bc || ! ppe_str_is_valid(s)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return ppe_false;
    }
    return ppe_sbc_copy_from_imp(bc, s, sz);
}

PPE_API ppe_bool ppe_sbc_copy_from(ppe_str_bunch restrict bc, const ppe_string restrict s)
{
    if (! bc || ! ppe_str_is_valid(s)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return ppe_false;
    }
    return ppe_sbc_copy_from_imp(bc, s->buf, s->len);
}

static ppe_string ppe_sbc_join_by_imp(ppe_str_bunch restrict bc, const char * restrict s, const ppe_size sz)
{
#define ppe_sbc_get_next_type(d, r, q, t) \
    do { \
        if (q == 0 || q == PPE_SBC_TYPE_COUNT_PER_CONTAINER) { \
            r = *d; \
            q = 0; \
            d += 1; \
        } \
        t = r & 0x3; \
        r >>= PPE_SBC_TYPE_BITS; \
        q += 1; \
    } while(0);

    ppe_string nw = NULL;
    ppe_size nwsz = 0;
    ppe_uint * d = NULL;
    ppe_uint r = 0;
    ppe_uint q = 0;
    ppe_uint t = 0;
    ppe_uint i = 0;

    if (bc->e.cnt == 0) {
        return &str_empty_s;
    }

    d = bc->t.type;
    r = 0;
    q = 0;
    t = 0;

    if (bc->e.cnt == 1) {
        ppe_sbc_get_next_type(d, r, q, t);
        switch (t) {
            case PPE_SBC_STR_REF:
                return ppe_cs_close(bc->e.ent[0]->s.buf, bc->e.ent[0]->s.len);

            default:
                return ppe_cs_close(bc->e.ent[0]->t.buf, bc->e.ent[0]->t.len);
        } /* switch */
    }

    for (i = 0; i < bc->e.cnt; i += 1) {
        ppe_sbc_get_next_type(d, r, q, t);
        switch (t) {
            case PPE_SBC_STR_REF:
                nwsz += bc->e.ent[i].s->len;
                break;

            default:
                nwsz += bc->e.ent[i].t.len;
                break;
        }
        nwsz += sz;
    }
    nwsz -= sz;

    if (nwsz == 0) {
        /* All the strings to join and the delimiter are empty. */
        return &str_empty_s;
    }

    nw = (ppe_string) ppe_mp_malloc(sizeof(ppe_string_st) + nwsz);
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    d = bc->t.type;
    r = 0;
    q = 0;
    t = 0;
    pos = nw->buf;
    switch (t) {
        case PPE_SBC_STR_REF:
            memcpy(pos, bc->e.ent[i].s->buf, bc->e.ent[i].s->len);
            pos += bc->e.ent[i].s->len;
            break;

        default:
            memcpy(pos, bc->e.ent[i].t->buf, bc->e.ent[i].t->len);
            pos += bc->e.ent[i].t->len;
            break;
    }

    for (i = 1; i < bc->e.cnt; i += 1) {
        memcpy(pos, s, sz);
        pos += sz;

        ppe_sbc_get_next_type(d, r, q, t);
        switch (t) {
            case PPE_SBC_STR_REF:
                memcpy(pos, bc->e.ent[i].s->buf, bc->e.ent[i].s->len);
                pos += bc->e.ent[i].s->len;
                break;

            default:
                memcpy(pos, bc->e.ent[i].t->buf, bc->e.ent[i].t->len);
                pos += bc->e.ent[i].t->len;
                break;
        }
    }
    *pos = '\0';
    nw->len = nwsz;
    return nw;

#undef ppe_sbc_get_next_type
}

PPE_API ppe_string ppe_sbc_join_by_cstr(ppe_str_bunch restrict bc, const char * restrict s, const ppe_size sz)
{
    if (! bc || ! ppe_str_is_valid(s)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return ppe_false;
    }
    return ppe_sbc_join_by_imp(bc, s, sz);
}

PPE_API ppe_string ppe_sbc_join_by(ppe_str_bunch restrict bc, const ppe_string restrict s)
{
    if (! bc || ! ppe_str_is_valid(s)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return ppe_false;
    }
    return ppe_sbc_join_by_imp(bc, s->buf, s->len);
}

PPE_API ppe_string ppe_sbc_concat(ppe_str_bunch restrict bc)
{
    if (! bc) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return ppe_false;
    }
    return ppe_sbc_join_by_imp(bc, str_empty_s.buf, str_empty_s.len);
}

PPE_API ppe_bool ppe_sbc_reference(ppe_str_bunch restrict bc, const ppe_uint idx, const char ** restrict s, const ppe_size * restrict sz)
{
    if (! bc || ! s || ! sz) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return ppe_false;
    }
    if (bc->e.cnt <= idx) {
        ppe_err_set(PPE_ERR_NO_SUCH_ENTRY, NULL);
        return ppe_false;
    }
    *s = bc->e.ent[idx].t.buf;
    *sz = bc->e.ent[idx].t.len;
    return ppe_true;
}

PPE_API ppe_int ppe_sbc_count(ppe_str_bunch restrict bc)
{
    if (! bc) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return -1;
    }
    return (ppe_int) bc->e.cnt;
}

#ifdef __cplusplus
}
#endif

