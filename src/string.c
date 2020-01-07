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

/* -- Internals -- */

static ppe_cstr ppe_cs_join_imp(ppe_cstr restrict b, ppe_size * restrict bsz, const ppe_cstr restrict d, ppe_size dsz, ppe_str_option opt, const ppe_bool enable_delimiter, va_list * restrict args)
{
    ppe_str_join_action act = PPE_PTR_JOIN_END;

    ppe_string str = NULL;
    ppe_cstr s = NULL;
    ppe_size sz = 0;
    ppe_size tsz = 0;
    ppe_size cpsz = 0;

    va_list cp;

PPE_PTR_JOIN_IMP_AGAIN:

    va_copy(cp, *args);

    act = va_arg(cp, ppe_str_join_action);
    if (act == PPE_PTR_JOIN_END) {
        goto PPE_PTR_JOIN_IMP_ERROR;
    }

    do {
        switch (act) {
            case PPE_PTR_JOIN_ADD_ITEM_CSTR:
                s = va_arg(cp, ppe_cstr);
                if (! s) {
                    goto PPE_PTR_JOIN_IMP_ERROR;
                }
                sz = ppe_cs_size(s);
                break;

            case PPE_PTR_JOIN_ADD_ITEM_CSTR_WITH_SIZE:
                s = va_arg(cp, ppe_cstr);
                if (! s) {
                    goto PPE_PTR_JOIN_IMP_ERROR;
                }
                sz = va_arg(cp, ppe_size);
                break;

            case PPE_PTR_JOIN_ADD_ITEM_STRING:
                str = va_arg(cp, ppe_string);
                if (! str) {
                    goto PPE_PTR_JOIN_IMP_ERROR;
                }
                sz = ppe_str_size(str);
                break;

            case PPE_PTR_JOIN_SET_DELIMITER_CSTR:
                /* Switch to a new delimiter in which next strings are being joined. */
                d = va_arg(cp, ppe_cstr);
                if (! d) {
                    goto PPE_PTR_JOIN_IMP_ERROR;
                }
                dsz = ppe_cs_size(d);
                goto PPE_PTR_JOIN_IMP_NONE_ITEM;

            case PPE_PTR_JOIN_SET_DELIMITER_CSTR_WITH_SIZE:
                /* Switch to a new delimiter in which next strings are being joined. */
                d = va_arg(cp, ppe_cstr);
                if (! d) {
                    goto PPE_PTR_JOIN_IMP_ERROR;
                }
                dsz = va_arg(cp, ppe_size);
                goto PPE_PTR_JOIN_IMP_NONE_ITEM;

            case PPE_PTR_JOIN_SET_DELIMITER_STRING:
                /* Switch to a new delimiter in which next strings are being joined. */
                str = va_arg(cp, ppe_string);
                if (! d) {
                    goto PPE_PTR_JOIN_IMP_ERROR;
                }
                d = ppe_str_addr(str);
                dsz = ppe_str_size(str);
                goto PPE_PTR_JOIN_IMP_NONE_ITEM;

            case PPE_PTR_JOIN_UNSET_DELIMITER:
                d = cs_empty_s;
                dsz = 0;
                goto PPE_PTR_JOIN_IMP_NONE_ITEM;

            default:
                goto PPE_PTR_JOIN_IMP_ERROR;
        } /* switch */

        if (! b) {
            tsz += sz;

            cnt += 1;
            if (enable_delimiter && cnt > 1) {
                tsz += dsz;
            }
        } else {
            if (sz > 0) {
                /* Copy as many bytes as the unused buffer can hold. */
                cpsz = *bsz - tsz;
                if (cpsz == 0) {
                    ppe_err_set(PPE_ERR_OUT_OF_BUFFER, NULL);
                    return NULL;
                }

                if (sz < cpsz) {
                    cpsz = sz;
                }
                memcpy(b + tsz, s, cpsz);
                tsz += cpsz;
            }

            cnt += 1;
            if (enable_delimiter && cnt > 1 && dsz > 0) {
                /* Copy as many bytes as the unused buffer can hold. */
                cpsz = *bsz - tsz;
                if (cpsz == 0) {
                    ppe_err_set(PPE_ERR_OUT_OF_BUFFER, NULL);
                    return ppe_false;
                }

                if (dsz < cpsz) {
                    cpsz = dsz;
                }
                memcpy(b + tsz, d, cpsz);
                tsz += cpsz;
            }
        } /* if */

PPE_PTR_JOIN_IMP_NONE_ITEM:
        act = va_arg(cp, ppe_str_join_action);
    } while (act != PPE_PTR_JOIN_END);

    va_end(cp);

    if (! b) {
        tsz += 1; /* Count the terminating NUL character. */
    } else {
        if (tsz + 1 < *bsz) {
            b[tsz] = '\0';
        }
    }
    *bsz = tsz;

    if (! b && opt & PPE_STR_NEW_STRING) {
        b = (ppe_cstr) ppe_mp_malloc(tsz + 1); /* Include the terminating NUL character. */
        if (! b) {
            ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
            return NULL;
        }
        goto PPE_PTR_JOIN_IMP_AGAIN;
    }
    return b;

PPE_PTR_JOIN_IMP_ERROR:
    va_end(cp);
    ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
    return NULL;
}

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

/* -- Substring -- */

PPE_API ppe_cstr ppe_cs_substr(ppe_cstr restrict s, const ppe_size index, const ppe_size bytes, ppe_str_option opt)
{
    ppe_size sz = 0;
    ppe_size cpsz = 0;

    assert(s);

    sz = ppe_cstr_size(s);
    if (sz <= index) {
        ppe_err_set(PPE_ERR_OUT_OF_RANGE, NULL);
        return NULL;
    }

    cpsz = (index + bytes <= sz) ? bytes : sz - index;

    if (opt & PPE_STR_OPT_NEW_STRING) {
        return ppe_cs_create(s + index, cpsz);
    }
    memmove(s, s + index, cpsz);
    return s;
}

/* -- Trim & Chomp -- */

PPE_API ppe_cstr ppe_cs_trim_bytes(ppe_cstr restrict s, const ppe_cstr restrict accept, ppe_str_option opt)
{
    ppe_cstr b = s;
    ppe_cstr p = NULL;
    ppe_size sz = 0;

    assert(s);
    assert(accept && ppe_cs_size(accept) > 0);

    if (opt & PPE_STR_OPT_DIRECT_LEFT) {
        while (*b && strchr(accept, *b)) {
            b++;
        }
    }

    sz = ppe_cs_size(b);
    if (sz == 0) {
        return cs_empty_s;
    }

    if (opt & PPE_STR_OPT_DIRECT_RIGHT) 
        p = b + sz - 1;
        while (b <= p && strchr(accept, *p)) {
            p--;
        }

        if (p < b) {
            return cs_empty_s;
        }
    } /* if */
    if (opt & PPE_STR_OPT_NEW_STRING) {
        return ppe_cs_create(s, p - b + 1);
    }
    memmove(s, b, p - b + 1);
    return s;
}

PPE_API ppe_cstr ppe_cs_chop(ppe_cstr restrict s, ppe_str_option opt)
{
    ppe_size sz = 0;

    assert(s);

    sz = ppe_cs_size(s);
    if (sz <= 1) {
        return cs_empty_s;
    }
    if (opt & PPE_STR_OPT_NEW_STRING) {
        return ppe_cs_create(s, sz - 1);
    }
    s[sz - 1] = '\0';
    return s;
}

PPE_API ppe_cstr ppe_cs_chomp(ppe_cstr restrict s, ppe_str_option opt)
{
    ppe_cstr p = 0;

    assert(s);

    p = strstr(s, PPE_STR_NEWLINE);
    if (opt & PPE_STR_OPT_NEW_STRING) {
        if (p) {
            return ppe_cs_create(s, p - s);
        }
        return ppe_cs_clone(s);
    }
    if (p) {
        p[0] = '\0';
    }
    return s;
}

/* -- Join & Concat -- */
 
PPE_API ppe_cstr ppe_cs_join(ppe_cstr restrict b, ppe_size * restrict bsz, const ppe_cstr restrict d, ppe_str_option opt, ...)
{
    ppe_cstr ret = NULL;
    va_list args;

    assert(d);
    assert(bsz);

    va_start(args, d);
    ret = ppe_cs_join_imp(b, bsz, d, ppe_cs_size(d), opt, ppe_true, &args);
    va_end(args);
    return ret;
}
 
PPE_API ppe_cstr ppe_cs_concat(ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt, ...)
{
    ppe_cstr ret = NULL;
    va_list args;

    assert(d);
    assert(bsz);

    va_start(args, bsz);
    ret = ppe_cs_join_imp(b, bsz, cs_empty_s, 0, opt, ppe_false, &args);
    va_end(args);
    return ret;
}

/* -- Split & Slice -- */

PPE_API ppe_bool ppe_cs_slice_into(ppe_char * restrict b, ppe_size * restrict bsz, const ppe_cstr restrict d, const ppe_cstr restrict s)
{
    const ppe_cstr p = NULL;
    ppe_size cpsz = 0;

    assert(bsz);
    assert(d);
    assert(s);

    if (d[0] == '\0') {
        /* Cannot use an empty string as delimiter. */
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return ppe_false;
    }

    p = ppe_cs_find(s, d);
    if (! p) {
        /* Cannot find a delimiter in the source string. */
        /* Take the whole source as a copying part. */
        cpsz = ppe_cs_size(s);
    } else if (s < p) {
        /* Found a delimiter which is not at the beginning. */
        cpsz = p - s;
    } /* if (s == p) {
        // Found a delimiter at the beginning.
        cpsz = 0;
    } */

    if (! b) {
        /* Detect the size of the part which is being sliced. */
        *bsz = cpsz;
        return ppe_true;
    }
    if (*bsz < cpsz) {
        ppe_err_set(PPE_ERR_OUT_OF_BUFFER, NULL);
        return ppe_false;
    }

    if (cpsz > 0) {
        memcpy(b, s, cpsz); /* Include no terminating NUL character. */
    }
    *bsz = cpsz;
    return ppe_true;
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

/* -- Join -- */

PPE_API ppe_string ppe_str_join(const ppe_string restrict d, ...)
{
    ppe_cstr ret = NULL;
    ppe_string nw = NULL;
    ppe_size nwsz = 0;
    va_list args;

    assert(d);

    va_start(args, d);
    ret = ppe_cs_join_imp(NULL, &nwsz, d->buf, d->sz, 0, ppe_true, &args);
    va_end(args);
    if (! ret) {
        return NULL;
    }

    nw = (ppe_string) ppe_mp_malloc(sizeof(ppe_string_st) + nwsz - 1); /* The terminating NUL charater has been counted twice. */
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    va_start(args, d);
    ret = ppe_cs_join_imp(nw->buf, &nwsz, d->buf, d->sz, 0, ppe_true, &args); /* Include the terminating NUL character. */
    va_end(args);

    if (! ret) {
        ppe_mp_free(nw);
        return NULL;
    }

    nw->sz = nwsz;
    return nw;
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
        goto PPE_PTR_JOIN_ERROR_HANDLING;
    }
    if (! ppe_sbc_push_refer_to(bc, s2)) {
        goto PPE_PTR_JOIN_ERROR_HANDLING;
    }

    do {
        if (! ppe_sbc_push_refer_to(bc, s)) {
            goto PPE_PTR_JOIN_ERROR_HANDLING;
        }

        s = va_arg(cp, ppe_string);
    } while (s != PPE_STR_ARG_END);

    nw = ppe_sbc_join_by_cstr_imp(bc, d, dsz);

PPE_PTR_JOIN_ERROR_HANDLING:
    ppe_sbc_clean(&bc);
    va_end(cp);
    return nw;
}

/* -- str module -- */

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

