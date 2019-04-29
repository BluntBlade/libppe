#include "ppe/mempool.h"
#include "ppe/string.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ==== Declaration : String ==== */

/* ---- Types ---- */

typedef struct PPE_STRING
{
    ppe_size sz;
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

/* split */

static ppe_string * ppe_cs_split_imp(const char * restrict d, const ppe_size dsz, const char * restrict s, const ppe_size sz, ppe_uint * restrict n)
{
    ppe_str_bunch bc = NULL;
    ppe_str_finder fd = NULL;
}

PPE_API extern ppe_string * ppe_cs_split_cstr(const char * restrict d, const ppe_size dsz, const char * restrict s, const ppe_size sz, ppe_uint * restrict n)
{
}

PPE_API ppe_string * ppe_cs_split(const char * restrict d, const ppe_size dsz, const ppe_string restrict s, ppe_uint * restrict n)
{
}

/* join */

static ppe_string ppe_cs_join_2_imp(const char * restrict d, ppe_size dsz, const char * restrict s1, const ppe_size sz1, const char * restrict s2, const ppe_size sz2)
{
    ppe_string nw = NULL;
    ppe_size rem = PPE_STR_MAX_LENGTH;

    if (sz1 > 0 && ! ppe_cs_subtract_remainder(&rem, sz1)) {
        ppe_err_set(PPE_ERR_OVERFLOW_UPPER_BOUND, NULL);
        return NULL;
    }
    if (sz2 > 0 && ! ppe_cs_subtract_remainder(&rem, sz2)) {
        ppe_err_set(PPE_ERR_OVERFLOW_UPPER_BOUND, NULL);
        return NULL;
    }

    if (rem == PPE_STR_MAX_LENGTH) return (dsz > 0) ? ppe_cs_clone(d, dsz) : &str_empty_s;

    if (dsz > 0 && ! ppe_cs_subtract_remainder(&rem, dsz)) {
        ppe_err_set(PPE_ERR_OVERFLOW_UPPER_BOUND, NULL);
        return NULL;
    }

    nw = (ppe_string) ppe_mp_malloc(sizeof(ppe_string_st) + PPE_STR_MAX_LENGTH - rem);
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }
    nw->sz = 0;

    if (sz1 > 0) {
        memcpy(nw->buf + nw->sz, s1, sz1);
        nw->sz += sz1;
    }
    if (dsz > 0) {
        memcpy(nw->buf + nw->sz, d, dsz);
        nw->sz += dsz;
    }
    if (sz2 > 0) {
        memcpy(nw->buf + nw->sz, s2, sz2);
        nw->sz += sz2;
    }

    nw->buf[nw->sz] = '\0';
    return nw;
}

static ppe_string ppe_cs_join_text_list_imp(const char * restrict deli, const ppe_size deli_len, ppe_text_st * restrict srcs, const ppe_uint n)
{
    ppe_uint i = 0;
    ppe_uint m = 0;
    ppe_size deli_len = 0;
    ppe_size remainder = 0;

    if (deli_len > 0) {
        if (srcs[0].sz > 0 && ! ppe_cs_subtract_remainder(&remainder, srcs[0].sz)) {
            ppe_err_set(PPE_ERR_OVERFLOW_UPPER_BOUND, NULL);
            return NULL;
        }
        for (i = 1; i < n; i += 1) {
            if (! ppe_cs_subtract_remainder(&remainder, deli_len)) {
                ppe_err_set(PPE_ERR_OVERFLOW_UPPER_BOUND, NULL);
                return NULL;
            }

            if (srcs[i].sz > 0 && ! ppe_cs_subtract_remainder(&remainder, srcs[i].sz)) {
                ppe_err_set(PPE_ERR_OVERFLOW_UPPER_BOUND, NULL);
                return NULL;
            }
        } /* for */
    } else {
        for (i = 0; i < n; i += 1) {
            if (srcs[i].sz == 0) continue;
            if (! ppe_cs_subtract_remainder(&remainder, srcs[i].sz)) {
                ppe_err_set(PPE_ERR_OVERFLOW_UPPER_BOUND, NULL);
                return NULL;
            }
            srcs[m].buf = srcs[i].buf;
            srcs[m].sz = srcs[i].sz;
            m += 1;
        } /* for */

        /* Concatenate empty strings. */
        if (m == 0) return &str_empty_s;
        if (m == 1) return ppe_cs_clone(srcs[0].buf, srcs[0].sz);
    }

    new_str = (ppe_string) ppe_mp_malloc(sizeof(ppe_string_st) + PPE_STR_MAX_LENGTH - remainder);
    if (! new_str) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }
    new_str->sz = 0;

    if (deli_len > 0) {
        memcpy(new_str->buf + new_str->sz, srcs[0].buf, srcs[0].sz);
        new_str->sz += srcs[0].sz;

        for (i = 1; i < n; i += 1) {
            memcpy(new_str->buf + new_str->sz, deli, deli_len);
            new_str->sz += deli_len;

            if (srcs[i].sz == 0) continue;
            memcpy(new_str->buf + new_str->sz, srcs[i].buf, srcs[i].sz);
            new_str->sz += srcs[i].sz;
        } /* for */
    } else {
        for (i = 0; i < m; i += 1) {
            memcpy(new_str->buf + new_str->sz, srcs[i].buf, srcs[i].sz);
            new_str->sz += srcs[i].sz;
        } /* for */
    }

    new_str[new_str->sz] = '\0';
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
    tmp_srcs[0].sz = strlen(s1);
    tmp_srcs[1].buf = s2;
    tmp_srcs[1].sz = strlen(s2);
    n = 2;

    do {
        if (n < PPE_STR_MAX_INPUT_N) {
            tmp_srcs[n].buf = src;
            tmp_srcs[n].sz = strlen(src);
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
        tmp_srcs[i].sz = strlen(srcs[i]);
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
    tmp_srcs[0].sz = ppe_cs_detect_size(s1, sz1);
    tmp_srcs[1].buf = s2;
    tmp_srcs[1].sz = ppe_cs_detect_size(s2, sz2);
    n = 2;

    do {
        if (n < PPE_STR_MAX_INPUT_N) {
            len = va_arg(ap, ppe_size);
            tmp_srcs[n].buf = src;
            tmp_srcs[n].sz = ppe_cs_detect_size(src, len);
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
    if (n == 1) return (srcs[0].sz > 0) ? ppe_cs_clone(srcs[0].buf, srcs[0].sz) : &str_empty_s;
    
    for (i = 0; i < n; i += 1) {
        tmp_srcs[i].buf = srcs[i].buf;
        tmp_srcs[i].sz = ppe_cs_detect_size(srcs[i].buf, srcs[i].sz);
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
        return ppe_cs_join_2_imp(deli, deli_len, s1->buf, s1->sz, s2->buf, s2->sz);
    }

    /* -- Prepare operands. -- */
    tmp_srcs[0].buf = s1->buf;
    tmp_srcs[0].sz = s1->sz;
    tmp_srcs[1].buf = s2->buf;
    tmp_srcs[1].sz = s2->sz;
    n = 2;

    do {
        if (n < PPE_STR_MAX_INPUT_N) {
            tmp_srcs[n].buf = src->buf;
            tmp_srcs[n].sz = src->sz;
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
    if (n == 1) return (srcs[i]->sz > 0) ? ppe_cs_clone(srcs[i]->buf, srcs[i]->sz) : &str_empty_s;

    for (i = 0; i < n; i += 1) {
        tmp_srcs[i].buf = srcs[i]->buf;
        tmp_srcs[i].sz = srcs[i]->sz;
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
    return ppe_cs_clone(src->buf, src->sz);
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
    ret = ppe_stc_find_cstr(trc, d->buf, d->sz, *n);
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

    while ((ret = ppe_stc_find_cstr(trc, d->buf, d->sz, *n - cnt)) > 0) {
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
    return ppe_str_split_cstr(d, s->buf, s->sz, n);
}

/* join */

PPE_API ppe_string ppe_str_join_cstr_var(const ppe_string restrict deli, const char * restrict s1, const char * restrict s2, va_list args)
{
    if (! ppe_str_is_valid(deli)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    return ppe_cs_join_cstr_var(deli->buf, deli->sz, s1, s2, args);
}

PPE_API ppe_string ppe_str_join_cstr_list(const ppe_string restrict deli, const char ** restrict srcs, const ppe_uint n)
{
    if (! ppe_str_is_valid(deli)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    return ppe_cs_join_cstr_list(deli->buf, deli->sz, srcs, n);
}

PPE_API ppe_string ppe_str_join_text_var(const ppe_string restrict deli, const char * restrict s1, const ppe_size sz1, const char * restrict s2, const ppe_size sz2, va_list args)
{
    if (! ppe_str_is_valid(deli)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    return ppe_cs_join_text_var(deli->buf, deli->sz, s1, sz1, s2, sz2, args);
}

PPE_API ppe_string ppe_str_join_text_list(const ppe_string restrict deli, const ppe_text_st * restrict srcs, const ppe_uint n)
{
    if (! ppe_str_is_valid(deli)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    return ppe_cs_join_cstr_list(deli->buf, deli->sz, srcs, n);
}

PPE_API ppe_string ppe_str_join_var(const ppe_string restrict deli, const ppe_string restrict s1, const ppe_string restrict s2, va_list args)
{
    if (! ppe_str_is_valid(deli)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    return ppe_cs_join_var(deli->buf, deli->sz, s1, s2, args);
}

PPE_API ppe_string ppe_str_join_list(const ppe_string restrict deli, const ppe_string * restrict srcs, const ppe_uint n)
{
    if (! ppe_str_is_valid(deli)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    return ppe_cs_join_list(deli->buf, deli->sz, srcs, n);
}

PPE_API ppe_string ppe_str_join_2(const ppe_string restrict deli, const ppe_string restrict s1, const ppe_string restrict s2)
{
    if (! ppe_str_is_valid(deli) || ! ppe_str_is_valid(s1) || ! ppe_str_is_valid(s2)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    return ppe_cs_join_2_imp(deli->buf, deli->sz, s1->buf, s1->sz, s2->buf, s2->sz);
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
    return str->sz;
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

#define PPE_SBC_BUFFER_SIZE 4096

static inline void ppe_sbc_init(ppe_str_bunch restrict bc)
{
    memset(bc, 0, sizeof(ppe_str_bunch_st));
    bc->b.cap = PPE_SBC_BUFFER_SIZE;
}

PPE_API ppe_str_bunch ppe_sbc_create(void)
{
    ppe_str_bunch bc = ppe_mp_malloc(sizeof(ppe_str_bunch_st));
    if (! bc) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }
    ppe_sbc_init(bc);
    return bc;
}

static void ppe_sbc_clean(ppe_str_bunch restrict bc)
{
    while (bc->b.cap > 0) {
        bc->b.cap -= 1;
        ppe_mp_free(bc->b.dat[bc->b.cap]);
    }
    ppe_mp_free(bc->t.type);
    ppe_mp_free(bc->e.ent);
}

PPE_API void ppe_sbc_destroy(ppe_str_bunch restrict bc)
{
    if (bc) {
        ppe_sbc_clean(bc);
        ppe_mp_free(bc);
    }
}

PPE_API void ppe_sbc_reset(ppe_str_bunch restrict bc)
{
    if (bc) {
        while (bc->b.cnt > 0) {
            bc->b.dat[--bc->b.cnt]->rem = PPE_SBC_BUFFER_SIZE;
        }
        bc->t.cnt = 0;
        bc->e.cnt = 0;
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
        bc->b.dat[bc->b.cnt] = (ppe_sbc_buffer) ppe_mp_malloc(PPE_SBC_BUFFER_SIZE);
        if (! bc->b.dat[bc->b.cnt]) {
            ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
            bc->bc.cnt -= 1;
            return ppe_false;
        }
    }
    bc->b.dat[bc->b.cnt]->rem = PPE_SBC_BUFFER_SIZE; /* TODO: Define a buffer size aligning to page size. */
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
    bc->e.ent[bc->e.cnt].t.sz =  sz;

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
    bc->e.ent[bc->e.cnt].t.buf = bc->b.dat[bc->b.cnt - 1]->buf + (PPE_SBC_BUFFER_SIZE - bc->b.dat[bc->b.cnt - 1]->rem);
    bc->e.ent[bc->e.cnt].t.sz = sz;
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
    return ppe_sbc_copy_from_imp(bc, s->buf, s->sz);
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
                return ppe_cs_close(bc->e.ent[0]->s.buf, bc->e.ent[0]->s.sz);

            default:
                return ppe_cs_close(bc->e.ent[0]->t.buf, bc->e.ent[0]->t.sz);
        } /* switch */
    }

    for (i = 0; i < bc->e.cnt; i += 1) {
        ppe_sbc_get_next_type(d, r, q, t);
        switch (t) {
            case PPE_SBC_STR_REF:
                nwsz += bc->e.ent[i].s->sz;
                break;

            default:
                nwsz += bc->e.ent[i].t.sz;
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
            memcpy(pos, bc->e.ent[i].s->buf, bc->e.ent[i].s->sz);
            pos += bc->e.ent[i].s->sz;
            break;

        default:
            memcpy(pos, bc->e.ent[i].t->buf, bc->e.ent[i].t->sz);
            pos += bc->e.ent[i].t->sz;
            break;
    }

    for (i = 1; i < bc->e.cnt; i += 1) {
        memcpy(pos, s, sz);
        pos += sz;

        ppe_sbc_get_next_type(d, r, q, t);
        switch (t) {
            case PPE_SBC_STR_REF:
                memcpy(pos, bc->e.ent[i].s->buf, bc->e.ent[i].s->sz);
                pos += bc->e.ent[i].s->sz;
                break;

            default:
                memcpy(pos, bc->e.ent[i].t->buf, bc->e.ent[i].t->sz);
                pos += bc->e.ent[i].t->sz;
                break;
        }
    }
    *pos = '\0';
    nw->sz = nwsz;
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
    return ppe_sbc_join_by_imp(bc, s->buf, s->sz);
}

PPE_API ppe_string ppe_sbc_concat(ppe_str_bunch restrict bc)
{
    if (! bc) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return ppe_false;
    }
    return ppe_sbc_join_by_imp(bc, str_empty_s.buf, str_empty_s.sz);
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
    *sz = bc->e.ent[idx].t.sz;
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

/* ==== Definition : String Finder ==== */

/* ---- Types ---- */

typedef struct PPE_STR_FINDER
{
    const char * pos;
    const char * end;
} ppe_str_finder_st;

/* ---- Functions ---- */

static inline void ppe_sfd_reset_for_imp(ppe_str_finder restrict fd, const char * restrict s, const ppe_size sz)
{
    if (sz == 0) {
        fd->pos = str_empty_s.buf;
        fd->end = str_empty_s.buf;
    } else {
        fd->pos = s;
        fd->end = s + sz;
    }
}

PPE_API ppe_str_finder ppe_sfd_create_for_cstr(const char * restrict s, const ppe_size sz)
{
    ppe_str_finder nw = NULL;

    if (! ppe_str_is_valid(s)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }

    nw = ppe_mp_malloc(sizeof(ppe_str_finder_st));
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }
    ppe_sfd_reset_for_imp(nw, s, sz);
    return nw;
}

PPE_API ppe_str_finder ppe_sfd_create_for(const ppe_string restrict s)
{
    if (! ppe_str_is_valid(s)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    return ppe_sfd_create_for_cstr(s->buf, s->sz);
}

PPE_API void ppe_sfd_destroy(ppe_str_finder restrict fd)
{
    if (fd) {
        ppe_mp_free(fd);
    }
}

PPE_API ppe_bool ppe_sfd_reset_for_cstr(ppe_str_finder restrict fd, const char * restrict s, const ppe_size sz)
{
    if (! fd || ! ppe_str_is_valid(s)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return ppe_false;
    }
    ppe_sfd_reset_for_imp(fd, s, sz);
    return ppe_true;
}

PPE_API ppe_bool ppe_sfd_reset_for(ppe_str_finder restrict fd, const ppe_string restrict s)
{
    if (! fd || ! ppe_str_is_valid(s)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return ppe_false;
    }
    ppe_sfd_reset_for_imp(fd, s->buf, s->sz);
    return ppe_true;
}

static const char * ppe_sfd_find_imp(ppe_str_finder restrict fd, const char * restrict s, const ppe_size sz)
{
    const char * pos = NULL;
    ppe_size rem = 0;

    assert(fd->pos <= fd->end);

    rem = fd->end - fd->pos;
    if (rem < sz) {
        /* The part to search of the string is shorter than the target substring. */
        return NULL;
    }

    /* If the search failed, don't consume any characters and leave them for the next search of other substrings. */

    if (sz == 1) {
        if ((pos = memchr(fd->pos, s[0], rem))) {
            fd->pos = pos + sz; /* Stop at the start point of the next search. */
            return pos;
        }
        return NULL;
    }

    for (pos = fd->pos; sz <= rem; ++pos, --rem) {
        pos = memchr(pos, s[0], rem);
        if (! pos) {
            return NULL;
        }

        if (memcmp(pos, s, sz) == 0) {
            fd->pos = pos + sz; /* Stop at the start point of the next search. */
            return pos;
        }
    } /* while */

    return NULL;
}

typedef enum
{
    PPE_SFD_FIND_TYPE_COMPONENT = 0,
    PPE_SFD_FIND_TYPE_SUBSTRING = 1
} ppe_sfd_find_type;

static ppe_bool ppe_sfd_find_n_imp(ppe_str_finder restrict fd, const char * s restrict, const ppe_size sz, ppe_uint * restrict n, ppe_str_bunch restrict bc, const ppe_sfd_find_type type)
{
    const char * begin = NULL;
    const char * pos = NULL;
    ppe_uint i = 0;
    ppe_uint max = ~0L;

    if (! fd || ! ppe_str_is_valid(s) || sz == 0) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return ppe_false;
    }

    if (n) {
        if (*n == 0) {
            ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
            return ppe_false;
        }
        if(*n < max) {
            max = *n;
        }
    }

    for (i = 0; i < max; ++i) {
        begin = fd->pos;

        pos = ppe_sfd_find_imp(fd, s, sz);
        if (! pos) {
            break;
        }

        if (bc) {
            if (type == PPE_SFD_FIND_TYPE_COMPONENT) {
                ret = ppe_sbc_refer_to_cstr(bc, begin, pos - begin);
            } else (type == PPE_SFD_FIND_TYPE_SUBSTRING) {
                ret = ppe_sbc_refer_to_cstr(bc, pos, sz);
            }

            if (! ret) {
                if (n) {
                    *n = i;
                }
                fd->pos = begin; /* Resume the start point of the next search. */
                return ppe_false;
            }
        }
    } /* for */
    if (n) {
        *n = i;
    }
    return ppe_true;
}

PPE_API ppe_bool ppe_sfd_find_component_by_cstr(ppe_str_finder restrict fd, const char * s restrict, const ppe_size sz, ppe_uint * restrict n, ppe_str_bunch restrict bc)
{
    return ppe_sfd_find_n_imp(fd, s, sz, n, bc, PPE_SFD_FIND_TYPE_COMPONENT);
}

PPE_API ppe_bool ppe_sfd_find_component_by(ppe_str_finder restrict fd, const ppe_string restrict s, ppe_uint * restrict n, ppe_str_bunch restrict bc)
{
    if (! ppe_str_is_valid(s)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return ppe_false;
    }
    return ppe_sfd_find_n_imp(fd, s->buf, s->sz, n, bc, PPE_SFD_FIND_TYPE_COMPONENT);
}

PPE_API ppe_bool ppe_sfd_refer_last_component(ppe_str_finder restrict fd, ppe_str_bunch restrict bc)
{
    if (! fd || ! bc) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return ppe_false;
    }
    return ppe_sbc_refer_to_cstr(bc, fd->pos, fd->end - fd->pos);
}

PPE_API ppe_bool ppe_sfd_find_substring_by_cstr(ppe_str_finder restrict fd, const char * s restrict, const ppe_size sz, ppe_uint * restrict n, ppe_str_bunch restrict bc)
{
    return ppe_sfd_find_n_imp(fd, s, sz, n, bc, PPE_SFD_FIND_TYPE_SUBSTRING);
}

PPE_API ppe_bool ppe_sfd_find_substring_by(ppe_str_finder restrict fd, const ppe_string restrict s, ppe_uint * restrict n, ppe_str_bunch restrict bc)
{
    if (! ppe_str_is_valid(s)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return ppe_false;
    }
    return ppe_sfd_find_n_imp(fd, s->buf, s->sz, n, bc, PPE_SFD_FIND_TYPE_SUBSTRING);
}

/* ==== Definitions : String ==== */

/* ---- Functions Defered to Implement ---- */

/* split */

static ppe_string * ppe_cs_split_imp(const char * restrict d, const ppe_size dsz, const char * restrict s, const ppe_size sz, ppe_uint * restrict n)
{
    ppe_str_bunch_st bc;
    ppe_str_finder_st fd;
    ppe_string * arr = NULL;
    const char * t = NULL;
    ppe_size tsz = 0;

    ppe_sbc_init(&bc);
    ppe_sfd_reset_for_cstr(s, sz);

    if (! ppe_sfd_find_n_imp(&fd, d, dsz, n, &bc, PPE_SFD_FIND_TYPE_COMPONENT)) {
        ppe_sbc_clean(&bc);
        return NULL;
    }

    arr = ppe_mp_malloc((ppe_sbc_count(&bc) + 1) * sizeof(ppe_string));
    if (! arr) {
        ppe_sbc_clean(&bc);
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    for (i = 0; i < ppe_sbc_count(&bc); ++i) {
        if (! ppe_sbc_reference(&bc, i, &t, &tsz)) {
            goto PPE_CS_SPLIT_IMP_ERROR_HANDLING;
        }
        arr[i] = ppe_cs_clone(t, tsz);
        if (! arr[i]) {
            goto PPE_CS_SPLIT_IMP_ERROR_HANDLING;
        }
    }
    arr[i] = NULL; /* Specify the end of the array. */
    return arr;

PPE_CS_SPLIT_IMP_ERROR_HANDLING:
    while (i > 0) {
        ppe_str_destroy(arr[--i]);
    }
    ppe_mp_free(arr);
    ppe_sbc_clean(&bc);
    return NULL;
}

PPE_API extern ppe_string * ppe_cs_split_cstr(const char * restrict d, const ppe_size dsz, const char * restrict s, const ppe_size sz, ppe_uint * restrict n)
{
    if (! ppe_str_is_valid(d) || dsz == 0 || ! ppe_str_is_valid(s)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (sz == 0) {
        return &str_empty_array_s;
    }
    return ppe_cs_split_imp(d, dsz, s, sz, n);
}

PPE_API ppe_string * ppe_cs_split(const char * restrict d, const ppe_size dsz, const ppe_string restrict s, ppe_uint * restrict n)
{
    if (! ppe_str_is_valid(d) || dsz == 0 || ! ppe_str_is_valid(s)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (s->sz == 0) {
        return &str_empty_array_s;
    }
    return ppe_cs_split_imp(d, dsz, s->buf, s->sz, n);
}

#ifdef __cplusplus
}
#endif

