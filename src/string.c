#include <string.h>

#include "ppe/mempool.h"
#include "ppe/string.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ==== Definitions : C-String ============================================== */

/* ---- Preset Values ------------------------------------------------------- */

static const ppe_char * cs_empty_s = "";

/* ---- Functions ----------------------------------------------------------- */

/* -- Preset values -- */

PPE_API const ppe_cstr ppe_cs_empty(void)
{
    return cs_empty_s;
}

/* -- Property -- */

PPE_API ppe_cs_size(const ppe_cstr restrict s)
{
    assert(s);
    return strlen(s);
}

/* -- Test -- */

PPE_API ppe_bool ppe_cs_is_empty(const ppe_cstr restrict s)
{
    assert(s);
    return (s[0] == '\0');
}

PPE_API ppe_int ppe_cs_compare(const ppe_cstr restrict s1, const ppe_cstr restrict s2)
{
    assert(s1);
    assert(s2);
    return strcmp(s1, s2);
}

/* -- Find -- */

PPE_API const ppe_cstr ppe_cs_find(const ppe_cstr restrict s, const ppe_cstr restrict t)
{
    assert(s);
    assert(t);
    return (const ppe_cstr) strstr(s, t);
}

/* -- Create & Destroy -- */

PPE_API void ppe_cs_destroy(ppe_cstr restrict s)
{
    if (s && s != cs_empty_s) {
        ppe_mp_free(s);
    }
}

PPE_API ppe_cstr ppe_cs_create(const ppe_cstr restrict s, const ppe_size sz)
{
    ppe_cstr nw = NULL;

    assert(s);

    if (ppe_cs_is_empty(s) || sz == 0) {
        return cs_empty_s;
    }

    nw = (ppe_cstr) ppe_mp_malloc(sz);
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    memcpy(nw, s, sz);
    nw[sz] = '\0';
    return nw;
}

/* -- Join & Concat -- */

PPE_API ppe_cstr ppe_cs_join(const ppe_cstr restrict d, const ppe_cstr restrict s1, const ppe_cstr restrict s2, ...)
{
}

PPE_API extern ppe_cstr ppe_cs_concat(const ppe_cstr restrict d, const ppe_cstr restrict s1, const ppe_cstr restrict s2, ...)
{
}

/* ==== Definitions : String ================================================ */

/* ---- Types --------------------------------------------------------------- */

typedef struct PPE_STRING
{
    ppe_ssize sz;
    ppe_char buf[1];
} ppe_string_st;

/* ---- Preset Values ------------------------------------------------------- */

static ppe_string_st str_empty_s = { 0, '\0' };

/* ---- Functions ----------------------------------------------------------- */

/* -- Internals -- */

static inline ppe_ssize ppe_cs_detect_size(const ppe_char * restrict s, const ppe_ssize sz)
{
    return (sz == PPE_STR_DETECT_SIZE) ? strlen(s) : sz;
}

static ppe_string ppe_cs_join_2_imp(const ppe_char * restrict d, ppe_ssize dsz, const ppe_char * restrict s1, const ppe_ssize sz1, const ppe_char * restrict s2, const ppe_ssize sz2)
{
    ppe_string nw = NULL;
    ppe_ssize rem = PPE_STR_MAX_SIZE;

    if ((rem -= sz1) < 0) {
        ppe_err_set(PPE_ERR_OVERFLOW_UPPER_BOUND, NULL);
        return NULL;
    }
    if ((rem -= dsz) < 0) {
        ppe_err_set(PPE_ERR_OVERFLOW_UPPER_BOUND, NULL);
        return NULL;
    }
    if ((rem -= sz2) < 0) {
        ppe_err_set(PPE_ERR_OVERFLOW_UPPER_BOUND, NULL);
        return NULL;
    }

    if (PPE_STR_MAX_SIZE - rem == dsz) {
        /* Both source strings are empty. */
        return ppe_cs_clone(d, dsz);
    }

    nw = (ppe_string) ppe_mp_malloc(sizeof(ppe_string_st) + (PPE_STR_MAX_SIZE - rem));
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

static ppe_string ppe_cs_join_imp(const ppe_char * restrict d, const ppe_ssize dsz, const ppe_char * restrict s1, const ppe_ssize sz1, const ppe_char * restrict s2, const ppe_ssize sz2, va_list args)
{
    va_list cp;
    ppe_str_bunch bc;
    ppe_string nw = NULL;
    const ppe_char * s = NULL;
    ppe_ssize sz = 0;
    ppe_uint n = 0;

    /* -- Check arguments. -- */
    va_copy(cp, args);

    s = va_arg(cp, const ppe_char *);
    if (s == PPE_STR_ARG_END) {
        /* Only two strings to join. */
        va_end(cp);
        return ppe_cs_join_2_imp(d, dsz, s1, sz1, s2, sz2);
    }

    /* -- Prepare operands. -- */
    ppe_sbc_init(&bc);

    if (! ppe_sbc_push_refer_to_cstr(bc, s1, sz1)) {
        goto PPE_CS_JOIN_IMP_ERROR_HANDLING;
    }
    if (! ppe_sbc_push_refer_to_cstr(bc, s2, sz2)) {
        goto PPE_CS_JOIN_IMP_ERROR_HANDLING;
    }

    do {
        sz = va_arg(cp, ppe_ssize);
        if (sz < 0) {
            ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
            goto PPE_CS_JOIN_IMP_ERROR_HANDLING;
        }
        if (! ppe_sbc_push_refer_to_cstr(bc, s, sz)) {
            goto PPE_CS_JOIN_IMP_ERROR_HANDLING;
        }

        s = va_arg(cp, const ppe_char *);
    } while (s != PPE_STR_ARG_END);

    nw = ppe_sbc_join_by_cstr_imp(bc, d, dsz);

PPE_CS_JOIN_IMP_ERROR_HANDLING:
    ppe_sbc_clean(&bc);
    va_end(cp);
    return nw;
}

/* -- Preset values -- */

PPE_API const ppe_string ppe_str_empty(void)
{
    return &str_empty_s;
}

/* -- Property -- */

PPE_API const ppe_char * pp_str_addr(ppe_string restrict s)
{
    assert(s != NULL);
    return s->buf;
}

PPE_API ppe_ssize ppe_str_size(ppe_string restrict s)
{
    assert(s != NULL);
    return s->sz;
}

/* -- Comparison -- */

PPE_API ppe_bool ppe_str_equals_to(const ppe_string restrict s1, const ppe_string restrict s2)
{
    assert(s1 != NULL);
    assert(s2 != NULL);
    return strcmp(s1->buf, s2->buf) == 0;
}

PPE_API ppe_bool ppe_str_less_than(const ppe_string restrict s1, const ppe_string restrict s2)
{
    assert(s1 != NULL);
    assert(s2 != NULL);
    return strcmp(s1->buf, s2->buf) < 0;
}

PPE_API ppe_bool ppe_str_greater_than(const ppe_string restrict s1, const ppe_string restrict s2)
{
    assert(s1 != NULL);
    assert(s2 != NULL);
    return strcmp(s1->buf, s2->buf) > 0;
}

/* -- Create & Destroy -- */

PPE_API void ppe_str_destroy(ppe_string restrict s)
{
    if (s && s != &str_empty_s) {
        ppe_mp_free(s);
    }
}

/* _cs_ series */

PPE_API ppe_string ppe_cs_clone(const ppe_char * restrict s, const ppe_ssize sz)
{
    ppe_string nw = NULL;

    assert(s != NULL && sz >= 0);

    if (sz == 0 || s[0] == '\0') {
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

PPE_API ppe_string ppe_cs_join(const ppe_char * restrict d, const ppe_ssize dsz, const ppe_char * restrict s1, const ppe_ssize sz1, const ppe_char * restrict s2, const ppe_ssize sz2, ...)
{
    va_list ap;
    ppe_string nw = NULL;

    assert(d != NULL && dsz >= 0);
    assert(s1 != NULL && sz1 >= 0);
    assert(s2 != NULL && sz2 >= 0);

    va_start(ap, sz2);
    nw = ppe_cs_join_imp(d, dsz, s1, sz1, s2, sz2, ap);
    va_end(ap);
    return nw;
}

PPE_API ppe_string ppe_cs_vsprintf(const ppe_char * restrict fmt, va_list args)
{
    /* TODO: Implementation */
    return NULL;
}

/* _str_ series */

/* -- cs module -- */

/* split */

static ppe_str_bunch ppe_cs_split_imp(const ppe_char * restrict d, const ppe_ssize dsz, const ppe_char * restrict s, const ppe_ssize sz, const ppe_int n)
{
    ppe_str_finder_st fd;
    ppe_str_bunch bcc = NULL;
    ppe_int ret = 0;

    bcc = ppe_sbc_create();
    if (! bcc) {
        return NULL;
    }

    ppe_sfd_reset_for_cstr(&fd, s, sz);

    ret = ppe_sfd_find_n_imp(&fd, d, dsz, n, ppe_false, bcc, NULL); 
    if (ret < 0) {
        ppe_sbc_destroy(bcc);
        return NULL;
    }
    if (! ppe_sfd_refer_last_component(&fd)) {
        ppe_sbc_destroy(bcc);
        return NULL;
    }
    return bcc;
}

PPE_API ppe_str_bunch ppe_cs_split(const ppe_char * restrict d, const ppe_ssize dsz, const ppe_char * restrict s, const ppe_ssize sz, const ppe_int n)
{
    assert(d != NULL && dsz > 0);
    assert(s != NULL);
    return ppe_cs_split_imp(d, dsz, s, sz, n);
}

PPE_API ppe_str_bunch ppe_cs_split_str(const ppe_char * restrict d, const ppe_ssize dsz, const ppe_string restrict s, const ppe_int n)
{
    assert(d != NULL && dsz > 0);
    assert(s != NULL);
    return ppe_cs_split_imp(d, dsz, s->buf, s->sz, n);
}

/* join */

static ppe_string ppe_cs_join_str_imp(const ppe_char * restrict d, const ppe_ssize dsz, const ppe_string restrict s1, const ppe_string restrict s2, va_list args)
{
    va_list cp;
    ppe_str_bunch bc;
    ppe_string nw = NULL;
    ppe_string s = NULL;
    ppe_uint n = 0;

    /* -- Check arguments. -- */
    va_copy(cp, args);

    s = va_arg(cp, ppe_string);
    if (s == PPE_STR_ARG_END) {
        /* Only two strings to join. */
        va_end(cp);
        return ppe_cs_join_2_imp(d, dsz, s1->buf, s1->sz, s2->buf, s2->sz);
    }

    /* -- Prepare operands. -- */
    ppe_sbc_init(&bc);

    if (! ppe_sbc_push_refer_to(bc, s1)) {
        goto PPE_CS_JOIN_ERROR_HANDLING;
    }
    if (! ppe_sbc_push_refer_to(bc, s2)) {
        goto PPE_CS_JOIN_ERROR_HANDLING;
    }

    do {
        if (! ppe_sbc_push_refer_to(bc, s)) {
            goto PPE_CS_JOIN_ERROR_HANDLING;
        }

        s = va_arg(cp, ppe_string);
    } while (s != PPE_STR_ARG_END);

    nw = ppe_sbc_join_by_cstr_imp(bc, d, dsz);

PPE_CS_JOIN_ERROR_HANDLING:
    ppe_sbc_clean(&bc);
    va_end(cp);
    return nw;
}

/* concat */

PPE_API ppe_string ppe_cs_concat(const ppe_char * restrict s1, const ppe_ssize sz1, const ppe_char * restrict s2, const ppe_ssize sz2, ...)
{
    va_list ap;
    ppe_string nw = NULL;

    assert(s1 != NULL && sz1 >= 0);
    assert(s2 != NULL && sz2 >= 0);

    va_start(ap, sz2);
    nw = ppe_cs_join_imp("", 0, s1, sz1, s2, sz2, ap);
    va_end(ap);
    return nw;
}

/* -- str module -- */

/* join */

PPE_API ppe_string ppe_str_join_cstr(const ppe_string restrict d, const ppe_char * restrict s1, const ppe_ssize sz1, const ppe_char * restrict s2, const ppe_ssize sz2, ...)
{
    va_list ap;
    ppe_string nw = NULL;

    assert(d != NULL);
    assert(s1 != NULL && sz1 >= 0);
    assert(s2 != NULL && sz2 >= 0);

    va_start(ap, sz2);
    nw = ppe_cs_join_imp(d->buf, d->sz, s1, sz1, s2, sz2, ap);
    va_end(ap);
    return nw;
}

PPE_API ppe_string ppe_str_join(const ppe_string restrict d, const ppe_string restrict s1, const ppe_string restrict s2, ...)
{
    va_list ap;
    ppe_string nw = NULL;

    assert(d != NULL);
    assert(s1 != NULL && sz1 >= 0);
    assert(s2 != NULL && sz2 >= 0);

    va_start(ap, s2);
    nw = ppe_cs_join_str_imp(d->buf, d->sz, s1, s2, ap);
    va_end(ap);
    return nw;
}

/* concat */

PPE_API ppe_string ppe_str_concat(const ppe_string restrict s1, const ppe_string restrict s2, ...)
{
    va_list ap;
    ppe_string nw = NULL;

    assert(s1 != NULL);
    assert(s2 != NULL);

    va_start(ap, s2);
    nw = ppe_cs_join_str_imp("", 0, s1, s2, ap);
    va_end(ap);
    return nw;
}

/* split */

PPE_API ppe_str_bunch ppe_str_split_cstr(const ppe_string restrict d, const ppe_char * restrict s, const ppe_ssize sz, const ppe_int n)
{
    assert(d != NULL && d->sz > 0);
    assert(s != NULL);
    return ppe_cs_split_imp(d->buf, d->sz, s, sz, n);
}

#ifdef __cplusplus
}
#endif

