#include <string.h>

#include "ppe/mempool.h"
#include "ppe/string.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ==== Declaration : C-String Snippet ====================================== */

/* ---- Types --------------------------------------------------------------- */

typedef struct PPE_CSPT_ITEM {
    const ppe_cstr s;
    ppe_size sz;
} ppe_cspt_item_st;

typedef struct PPE_CS_SNIPPET {
    ppe_uint cnt;
    ppe_uint cap;
    ppe_cspt_item_st items[1];
} ppe_cs_snippet_st;

/* ---- Preset Values ------------------------------------------------------- */

static const ppe_char * cs_empty_s;
static const ppe_cs_snippet_st cspt_empty_s = {1, 1, {cs_empty_s, 0}};

/* -- Property -- */

PPE_API ppe_uint ppe_cspt_count(const ppe_cs_snippet restrict spt)
{
    return spt->cnt;
}

PPE_API ppe_uint ppe_cspt_capacity(const ppe_cs_snippet restrict spt)
{
    return spt->cap;
}

/* -- Create & Destroy -- */

PPE_API ppe_cs_snippet ppe_cspt_create(const ppe_uint cap)
{
    ppe_cs_snippet nw = NULL;

    if (cap == 0) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }

    nw = ppe_mp_malloc(sizeof(ppe_cs_snippet_st) + sizeof(ppe_cspt_item_st) * (cap - 1));
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }
    nw->cnt = 0;
    nw->cap = cap;
    return nw;
}

PPE_API void ppe_cspt_destroy(ppe_cs_snippet restrict spt)
{
    if (spt && spt != cspt_empty_s) {
        ppe_mp_free(spt);
    }
}

PPE_API void ppe_cspt_reset(ppe_cs_snippet restrict spt)
{
    spt->cnt = 0;
}

/* -- Manipulators -- */

PPE_API ppe_bool ppe_cspt_get(ppe_cs_snippet restrict spt, const ppe_uint idx, const ppe_cstr * restrict s, ppe_size * restrict sz)
{
    if (! spt || ! s || ! sz) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return ppe_false;
    }
    if (spt->cnt < idx) {
        ppe_err_set(PPE_ERR_OUT_OF_RANGE, NULL);
        return ppe_false;
    }

    *s = spt->items[idx].s;
    *sz = spt->items[idx].sz;
    return ppe_true;
}

PPE_API ppe_bool ppe_cspt_append(ppe_cs_snippet restrict spt, const ppe_cstr restrict s, const ppe_size sz)
{
    if (! spt) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return ppe_false;
    }
    if (spt->cnt == spt->cap) {
        ppe_err_set(PPE_ERR_OUT_OF_CAPACITY, NULL);
        return ppe_false;
    }

    spt->items[spt->cnt].s = s;
    spt->items[spt->cnt].sz = sz;
    spt->cnt += 1;
    return ppe_true;
}

/* -- Producer -- */

PPE_API ppe_cstr ppe_cspt_clone(ppe_cs_snippet restrict spt, const ppe_uint idx, ppe_cstr restrict b, ppe_size * bsz)
{
    const ppe_cstr s = NULL;
    ppe_size cpsz = 0;

    if (! spt) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (spt->cnt <= idx) {
        ppe_err_set(PPE_ERR_OUT_OF_RANGE, NULL);
        return NULL;
    }

    s = spt->items[idx].s;
    cpsz = spt->items[idx].sz;

    if (! b) {
        if (bsz) {
            /* MEASURE-SIZE MODE */
            *bsz = cpsz;
            ppe_err_set(PPE_ERR_CALL_AGAIN, NULL);
            return NULL;
        }
        /* NEW-STRING MODE */
        return ppe_cs_create(s, cpsz);
    } /* if */

    /* FILL-BUFFER MODE */
    if (! bsz) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (*bsz < cpsz + 1) {
        ppe_err_set(PPE_ERR_OUT_OF_CAPACITY, NULL);
        return NULL;
    }
    memcpy(b, s, cpsz);
    b[cpsz] = '\0';
    *bsz = cpsz;
    return b;
} /* ppe_cspt_clone */

PPE_API ppe_cstr * ppe_cspt_clone_some(const ppe_cs_snippet restrict spt, const ppe_uint idx, ppe_uint * restrict n)
{
    ppe_cstr * nw = NULL;
    ppe_uint max = 0;
    ppe_uint i = 0;

    if (! spt) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (spt->cnt <= idx) {
        ppe_err_set(PPE_ERR_OUT_OF_RANGE, NULL);
        return NULL;
    }

    if (n && *n > 0) {
        max = spt->cnt < idx + *n ? spt->cnt - idx : *n;
    } else {
        max = spt->cnt;
    }

    nw = (ppe_cstr *) malloc(sizeof(ppe_cstr) * (max + 1));
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    for (i = 0; i < max; i += 1) {
        nw[i] = ppe_cs_create(spt->items[idx + i].s, spt->items[idx + i].sz);
        if (! nw[i]) {
            while (i > 0) { ppe_cs_destroy(nw[--i]); }
            ppe_mp_free(nw);
            return NULL;
        } /* if */
    } /* for */
    nw[max] = NULL;
    if (n) {
        *n = max;
    }
    return nw;
} /* ppe_cspt_clone_some */

/* ==== Definitions : C-String ============================================== */

/* ---- Preset Values ------------------------------------------------------- */

static const ppe_char * cs_empty_s = "";

/* ---- Functions ----------------------------------------------------------- */

/* -- Internals -- */

static ppe_bool ppe_cs_join_imp_copy(const ppe_cstr restrict s, const ppe_size sz, const ppe_cstr restrict d, const ppe_size dsz, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_size * restrict tsz, ppe_uint * restrict cnt)
{
    if (cnt && ++*cnt > 1) {
        if (*bsz - *tsz < dsz) {
            ppe_err_set(PPE_ERR_OUT_OF_CAPACITY, NULL);
            return ppe_false;
        }
        memcpy(b + *tsz, d, dsz);
        *tsz += dsz;
    } /* if */

    if (*bsz - *tsz < sz) {
        ppe_err_set(PPE_ERR_OUT_OF_CAPACITY, NULL);
        return ppe_false;
    }
    memcpy(b + *tsz, s, sz);
    *tsz += sz;
    return ppe_true;
}

static ppe_cstr ppe_cs_join_imp(const ppe_cstr restrict d, const ppe_size dsz, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt, const ppe_bool enable_delimiter, va_list * restrict args)
{
    ppe_str_join_action act = PPE_PTR_JOIN_END;

    ppe_string str = NULL;
    ppe_cstr nw = NULL;
    ppe_cstr s = NULL;
    ppe_size sz = 0;
    ppe_size sz_r = 0;
    ppe_size nwsz = 0;
    ppe_size cpsz = 0;
    ppe_uint i = 0;
    va_list cp;

    if (b) {
        if (! bsz || *bsz == 0) {
            ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
            return NULL;
        }

        nw = b;
        nwsz = *bsz;
    }

PPE_PTR_JOIN_IMP_AGAIN:

    va_copy(cp, *args);

    act = va_arg(cp, ppe_str_join_action);
    if (act == PPE_PTR_JOIN_END) {
        goto PPE_CS_JOIN_IMP_ERROR_HANDLING_ARGUMENT;
    }

    do {
        switch (act) {
            case PPE_PTR_JOIN_ADD_ITEM_CSTR:
                s = va_arg(cp, ppe_cstr);
                if (! s) {
                    goto PPE_CS_JOIN_IMP_ERROR_HANDLING_ARGUMENT;
                }
                sz = ppe_cs_size(s);
                break;

            case PPE_PTR_JOIN_ADD_ITEM_CSTR_WITH_SIZE:
                s = va_arg(cp, ppe_cstr);
                if (! s) {
                    goto PPE_CS_JOIN_IMP_ERROR_HANDLING_ARGUMENT;
                }
                sz = va_arg(cp, ppe_size);
                break;

            case PPE_PTR_JOIN_ADD_ITEM_STRING:
                str = va_arg(cp, ppe_string);
                if (! str) {
                    goto PPE_CS_JOIN_IMP_ERROR_HANDLING_ARGUMENT;
                }
                s = ppe_str_addr(str);
                sz = ppe_str_size(str);
                break;

            case PPE_PTR_JOIN_ADD_ITEM_SNIPPET:
                spt = va_arg(cp, ppe_cs_snippet);
                if (! spt) {
                    goto PPE_CS_JOIN_IMP_ERROR_HANDLING_ARGUMENT;
                }
                sz = 0;
                for (i = 0; i < ppe_cspt_count(spt); i += 1) {
                    ppe_cspt_get(spt, i, &s, &sz_r);
                    sz += sz_r;
                } /* for */
                if (enable_delimiter && i > 1) {
                    cnt += (i - 1);
                    sz += dsz * (i - 1);
                }
                break;

            case PPE_PTR_JOIN_SET_DELIMITER_CSTR:
                /* Switch to a new delimiter in which next strings are being joined. */
                d = va_arg(cp, ppe_cstr);
                if (! d) {
                    goto PPE_CS_JOIN_IMP_ERROR_HANDLING_ARGUMENT;
                }
                dsz = ppe_cs_size(d);
                goto PPE_PTR_JOIN_IMP_NONE_ITEM;

            case PPE_PTR_JOIN_SET_DELIMITER_CSTR_WITH_SIZE:
                /* Switch to a new delimiter in which next strings are being joined. */
                d = va_arg(cp, ppe_cstr);
                if (! d) {
                    goto PPE_CS_JOIN_IMP_ERROR_HANDLING_ARGUMENT;
                }
                dsz = va_arg(cp, ppe_size);
                goto PPE_PTR_JOIN_IMP_NONE_ITEM;

            case PPE_PTR_JOIN_SET_DELIMITER_STRING:
                /* Switch to a new delimiter in which next strings are being joined. */
                str = va_arg(cp, ppe_string);
                if (! str) {
                    goto PPE_CS_JOIN_IMP_ERROR_HANDLING_ARGUMENT;
                }
                d = ppe_str_addr(str);
                dsz = ppe_str_size(str);
                goto PPE_PTR_JOIN_IMP_NONE_ITEM;

            case PPE_PTR_JOIN_UNSET_DELIMITER:
                d = cs_empty_s;
                dsz = 0;
                goto PPE_PTR_JOIN_IMP_NONE_ITEM;

            default:
                goto PPE_CS_JOIN_IMP_ERROR_HANDLING_ARGUMENT;
        } /* switch */

        if (! nw) {
            nwsz += sz;
            if (enable_delimiter && ++cnt > 1) {
                cpsz += dsz;
            }
        } else if (act == PPE_PTR_JOIN_ADD_ITEM_SNIPPET) {
            for (i = 0; i < ppe_cspt_count(spt); i += 1) {
                ppe_cspt_get(spt, i, &s, &sz);
                if (! ppe_cs_join_imp_copy(s, sz, d, dsz, nw, nwsz, &cpsz, (enable_delimiter ? &cnt : NULL))) {
                    goto PPE_CS_JOIN_IMP_ERROR_HANDLING;
                }
            } /* for */
        } else if (! ppe_cs_join_imp_copy(s, sz, d, dsz, nw, nwsz, &cpsz, (enable_delimiter ? &cnt : NULL))) {
            goto PPE_CS_JOIN_IMP_ERROR_HANDLING;
        } /* if */

PPE_PTR_JOIN_IMP_NONE_ITEM:
        act = va_arg(cp, ppe_str_join_action);
    } while (act != PPE_PTR_JOIN_END);

    va_end(cp);

    if (! nw) {
        if (bsz) {
            /* MEASURE-SIZE MODE */
            *bsz = nwsz + 1; /* Include the terminating NUL character. */
            ppe_err_set(PPE_ERR_CALL_AGAIN, NULL);
            return NULL;
        }

        /* NEW-STRING MODE */
        nw = (ppe_cstr) ppe_mp_malloc(nwsz + 1); /* Include the terminating NUL character. */
        if (! nw) {
            ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
            return NULL;
        }
        goto PPE_PTR_JOIN_IMP_AGAIN;
    } /* if */

    if (! bsz) {
        /* NEW-STRING MODE */
        nw[cpsz] = '\0';
    } else {
        /* FILL-BUFFER MODE */
        if (cpsz + 1 < *bsz) {
            nw[cpsz] = '\0';
        }
        *bsz = cpsz;
    } /* if */
    return nw;

PPE_CS_JOIN_IMP_ERROR_HANDLING_ARGUMENT:
    ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);

PPE_CS_JOIN_IMP_ERROR_HANDLING:
    if (! b) {
        ppe_mp_free(nw);
        return NULL;
    }
    va_end(cp);
    return NULL;
}

#define PPE_CONF_STR_SNIPPET_MAX 64

static ppe_cs_snippet ppe_cs_split_imp(const ppe_cstr restrict s, const ppe_cstr restrict d, const ppe_size dsz, ppe_cs_snippet restrict spt, ppe_uint * restrict n, const ppe_cstr * restrict ss, const ppe_str_option opt)
{
    ppe_cs_snippet nw = NULL;
    ppe_cstr p = 0;
    ppe_cstr q = 0;
    ppe_size sz = 0;
    ppe_uint i = 0;
    ppe_uint max = PPE_CONF_STR_SNIPPET_MAX; /* Dont search infinitely. */

    if (n) {
        if (n == 0) {
            /* Cannot use a null or empty string as delimiter. */
            ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
            return NULL;
        }
        max = *n;
    }

    p = (ss && *ss) ? *ss : s;
    if (! spt) {
        /* NEW-SNIPPET MODE */
        if (ppe_cs_is_empty(p)) {
            return cspt_empty_s;
        }

        /* Count the number of substrings. */
        for (i = 0; i < max - 1 && (q = ppe_cs_find(p, d)); i += 1) {
            p = q + dsz;
        } /* for */

        /* Counted N delimiters, so there must be N+1 substrings, including empty ones. */
        i += 1;

        nw = ppe_cspt_create(i);
        if (! nw) {
            ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
            return NULL;
        }

        p = (ss && *ss) ? *ss : s;
    } else {
        /* FILL-SNIPPET MODE */
        ppe_cspt_reset(spt);
        if (ppe_cs_is_empty(p)) {
            ppe_cspt_append(spt, cs_empty_s, 0);
            return spt;
        }

        i = ppe_cspt_capacity(spt);
        nw = spt;
    } /* if */

    if (i < max) {
        max = i;
    }
    for (i = 0; i < max - 1 && (q = ppe_cs_find(p, d)); i += 1) {
        sz = q - p;
        ppe_cspt_append(nw, p, sz);
        p = q + dsz;
    } /* for */

    if (i == max - 1 && (q = ppe_cs_find(p, d)) && ss) {
        sz = q - p;
        ppe_cspt_append(nw, p, sz);
        p += sz;
        *ss = p;
    } else {
        sz = ppe_cs_size(p);
        ppe_cspt_append(nw, p, sz);
        p += sz;
        if (ss) {
            *ss = NULL;
        }
    } /* if */
    if (n) {
        *n = i + 1;
    }
    return nw;
} /* ppe_cs_split_imp */

static ppe_cs_cstr ppe_cs_replace_imp(const ppe_cstr restrict s, const ppe_size sz, const ppe_size off, const ppe_size ssz, const ppe_cstr restrict to, const ppe_size tosz, ppe_cs_cstr restrict b, ppe_size * restrict bsz, const ppe_str_option opt)
{
    ppe_size cpsz = 0;

    if (sz < off) {
        ppe_err_set(PPE_ERR_OUT_OF_RANGE, NULL);
        return NULL;
    }

    if (sz < off + ssz) {
        /* Shrink the range of the substring. */
        ssz -= (off + ssz) - sz;
    }
    cpsz = sz - ssz + tosz;

    if (! b) {
        if (bsz) {
            /* MEASURE MODE */
            *bsz = cpsz + 1; /* Include the terminating NUL byte. */
            return s;
        }

        /* NEW-STRING MODE */
        b = ppe_mp_malloc(cpsz + 1); /* Include the terminating NUL byte. */
        if (! b) {
            ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
            return NULL;
        }
    } else {
        /* FILL-BUFFER MODE */
        if (! bsz) {
            ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
            return NULL;
        }
        if (*bsz < cpsz + 1) {
            ppe_err_set(PPE_ERR_OUT_OF_CAPACITY, NULL);
            return NULL;
        }
        *bsz = cpsz;
    } /* if */

    /* The part before the substring which is being replaced. */
    memcpy(b, s, off);

    /* The part which is replacing the substring. */
    memcpy(b + off, to, tosz);

    /* The part after the substring which is being replaced. */
    memcpy(b + off + tosz, s + (off + ssz), sz - (off + ssz));

    b[cpsz] = '\0';
    return b;
} /* ppe_cs_replace_imp */

#define PPE_CONF_STR_SUBSTITUTE_MAX 256

static ppe_cs_cstr ppe_cs_substitute_imp(const ppe_cstr restrict s, const ppe_cstr restrict from, const ppe_size frsz, const ppe_cstr restrict to, const ppe_size tosz, ppe_cs_cstr restrict b, ppe_size * restrict bsz, ppe_uint * restrict n, const ppe_str_option opt)
{
    const ppe_cstr p = NULL;
    const ppe_cstr q = NULL;
    ppe_size rmsz = 0;
    ppe_size cpsz = 0;
    ppe_uint i = 0;
    ppe_uint max = PPE_CONF_STR_SUBSTITUTE_MAX;

    if (n) {
        if (*n == 0) {
            ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
            return NULL;
        }
        if (*n < max) {
            max = *n;
        }
    } /* if */

    if (! b) {
        p = s;
        if (frsz != tosz) {
            for (i = 0; i < max && (q = ppe_cs_find(p, from)); i += 1, p = q + frsz) {
                cpsz += (q - p) + tosz;
            } /* while */
        } /* if */

        rmsz = ppe_cs_size(p);
        cpsz += rmsz;

        if (bsz) {
            /* MEASURE-SIZE MODE */
            *bsz = cpsz + 1; /* Include the terminating NUL byte. */
            if (n) { *n = i; }
            ppe_err_set(PPE_ERR_CALL_AGAIN, NULL);
            return NULL;
        } /* if */

        /* NEW-STRING MODE */
        if (i == 0) {
            /* Subtitute no substrings. */
            if (n) { *n = i; }
            return ppe_cs_create(s, cpsz);
        } /* if */

        b = (ppe_cstr) ppe_mp_malloc(cpsz + 1); /* Include the terminating NUL byte. */
        if (! b) {
            ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
            return NULL;
        }
        cpsz = 0;
        max = i;
    } else {
        /* FILL-BUFFER */
        if (b == s || b == from || b == to) {
            ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
            return NULL;
        }
        if (! bsz) {
            ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
            return NULL;
        }
    } /* if */

    p = s;
    for (i = 0; i < max && (q = ppe_cs_find(p, from)); i += 1, p = q + frsz) {
        if (*bsz < cpsz + (q - s)) {
            ppe_err_set(PPE_ERR_OUT_OF_CAPACITY, NULL);
            return NULL;
        }
        memcpy(b + cpsz, p, q - p);
        cpsz += q - p;

        if (*bsz < cpsz + tosz) {
            ppe_err_set(PPE_ERR_OUT_OF_CAPACITY, NULL);
            return NULL;
        }
        memcpy(b + cpsz, to, tosz);
        cpsz += tosz;
    } /* for */

    rmsz = ppe_cs_size(p);
    if (*bsz < cpsz + rmsz + 1) {
        ppe_err_set(PPE_ERR_OUT_OF_CAPACITY, NULL);
        return NULL;
    }
    memcpy(b + cpsz, p, rmsz);
    cpsz += rmsz;

    b[cpsz] = '\0';
    if (bsz) { *bsz = cpsz; }
    if (n) { *n = i; }
    return b;
} /* ppe_cs_substitute_imp */

/* -- Preset values -- */

PPE_API const ppe_cstr ppe_cs_empty(void)
{
    return cs_empty_s;
}

/* -- Property -- */

PPE_API ppe_size ppe_cs_size(const ppe_cstr restrict s)
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

    if (! s) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }

    if (ppe_cs_is_empty(s) || sz == 0) {
        return cs_empty_s;
    }

    nw = (ppe_cstr) ppe_mp_malloc(sz + 1); /* Include the terminating NUL byte. */
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    memcpy(nw, s, sz);
    nw[sz] = '\0';
    return nw;
}

/* -- Substring -- */

PPE_API ppe_cstr ppe_cs_substr(const ppe_cstr restrict s, const ppe_size off, const ppe_size ssz, ppe_cstr restrict b, ppe_size * bsz, ppe_str_option opt)
{
    ppe_size sz = 0;
    ppe_size cpsz = 0;

    if (! s) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }

    sz = ppe_cs_size(s);
    if (sz <= off) {
        ppe_err_set(PPE_ERR_OUT_OF_RANGE, NULL);
        return NULL;
    }

    cpsz = sz < off + ssz ? sz - off : ssz;
    
    if (! b) {
        if (bsz) {
            /* MEASURE-SIZE MODE */
            *bsz = cpsz + 1; /* Include the terminating NUL byte. */
            ppe_err_set(PPE_ERR_CALL_AGAIN, NULL);
            return NULL;
        }
        
        /* NEW-STRING MODE */
        return ppe_cs_create(s + off, cpsz);
    } /* if */

    if (b == s) {
        /* IN-PLACE MODE */
        if (off > 0) {
            memmove(b, s + off, cpsz);
        }
        b[cpsz] = '\0';
        if (bsz) {
            *bsz = cpsz;
        }
        return b;
    } /* if */

    /* FILL-BUFFER MODE */
    if (! bsz) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (*bsz < cpsz + 1) {
        ppe_err_set(PPE_ERR_OUT_OF_CAPACITY, NULL);
        return NULL;
    }
    memcpy(b, s + off, cpsz);
    b[cpsz] = '\0';
    *bsz = cpsz;
    return b;
} /* ppe_cs_substr */

/* -- Trim & Chomp -- */

PPE_API ppe_cstr ppe_cs_trim_bytes(const ppe_cstr restrict s, const ppe_cstr restrict t, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt)
{
    const ppe_cstr p = NULL;
    const ppe_cstr q = NULL;
    ppe_size cpsz = 0;

    if (! s || ! t || ppe_cs_size(t) == 0) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }

    p = s;
    if (opt & PPE_STR_OPT_DIRECT_LEFT) {
        while (*p && strchr(t, *p)) {
            p++;
        }
    } /* if */

    cpsz = ppe_cs_size(p);
    if (opt & PPE_STR_OPT_DIRECT_RIGHT) {
        q = p + cpsz - 1;
        while (p <= q && strchr(t, *q)) {
            q--;
        }
        cpsz = q < p ? 0 : q - p + 1;
    } /* if */

    if (! b) {
        if (bsz) {
            /* MEASURE-SIZE MODE */
            *bsz = cpsz + 1; /* Include the terminating NUL byte. */
            ppe_err_set(PPE_ERR_CALL_AGAIN, NULL);
            return NULL;
        }
        /* NEW-STRING MODE */
        return cpsz > 0 ? ppe_cs_create(p, cpsz) : cs_empty_s;
    }

    if (b == s) {
        /* IN-PLACE MODE */
        if (s < p) {
            memmove(b, p, cpsz);
        }
        b[cpsz] = '\0';
        if (bsz) {
            *bsz = cpsz;
        }
        return b;
    } /* if */

    /* FILL-BUFFER MODE */
    if (! bsz) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (*bsz < cpsz + 1) {
        ppe_err_set(PPE_ERR_OUT_OF_CAPACITY, NULL);
        return NULL;
    }
    memcpy(b, p, cpsz);
    b[cpsz] = '\0';
    *bsz = cpsz;
    return b;
} /* ppe_cs_trim_bytes */

PPE_API ppe_cstr ppe_cs_chop(const ppe_cstr restrict s, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt)
{
    ppe_size cpsz = 0;

    if (! s) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }

    cpsz = ppe_cs_size(s);
    if (! b) {
        if (bsz) {
            /* MEASURE-SIZE MODE */
            *bsz = cpsz; /* Include the terminating NUL byte. */
            ppe_err_set(PPE_ERR_CALL_AGAIN, NULL);
            return NULL;
        }
        /* NEW-STRING MODE */
        return cpsz > 0 ? ppe_cs_create(s, cpsz - 1) : ppe_cs_empty;
    } /* if */

    if (b == s) {
        /* IN-PLACE MODE */
        if (cpsz > 0) {
            b[cpsz - 1] = '\0';
        }
        if (bsz) {
            *bsz = cpsz > 0 ? cpsz - 1 : 0;
        }
        return b;
    }

    /* FILL-BUFFER MODE */
    if (! bsz) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (cpsz > 0) {
        if (*bsz < cpsz) {
            ppe_err_set(PPE_ERR_OUT_OF_CAPACITY, NULL);
            return NULL;
        }
        memcpy(b, s, cpsz - 1);
    } else if (*bsz == 0) {
        ppe_err_set(PPE_ERR_OUT_OF_CAPACITY, NULL);
        return NULL;
    } /* if */
    b[cpsz] = '\0';
    *bsz = cpsz;
    return b;
} /* ppe_cs_chop */

static ppe_cs_chomp_imp(const ppe_cstr restrict s, const ppe_size sz, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt)
{
    const ppe_cstr p = NULL;
    const ppe_size nlsz = strlen(PPE_STR_NEWLINE);
    ppe_size cpsz = 0;

    p = ppe_cs_find(s + sz - nlsz, PPE_STR_NEWLINE);
    if (p) {
        cpsz = p - s; /* Exclude the terminating NUL byte. */
    } else {
        cpsz = sz;
    }

    if (! b) {
        if (bsz) {
            /* MEASURE-SIZE MODE */
            *bsz = cpsz + 1; /* Include the terminating NUL byte. */
            ppe_err_set(PPE_ERR_CALL_AGAIN, NULL);
            return NULL;
        }
        /* NEW-STRING MODE */
        return ppe_cs_create(s, cpsz);
    } /* if */

    if (b == s) {
        /* IN-PLACE MODE */
        if (p) {
            p[0] = '\0';
        }
        if (bsz) {
            *bsz = cpsz;
        }
        return b;
    } /* if */

    /* FILL-BUFFER */
    if (! bsz) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (*bsz < cpsz + 1) {
        ppe_err_set(PPE_ERR_OUT_OF_CAPACITY, NULL);
        return NULL;
    }
    memcpy(b, s, cpsz);
    b[cpsz] = '\0';
    *bsz = cpsz;
    return b;
} /* ppe_cs_chomp_imp */

PPE_API ppe_cstr ppe_cs_chomp(const ppe_cstr restrict s, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt)
{
    if (! s) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    return ppe_cs_chomp(s, ppe_cs_size(s), b, bsz, opt);
} /* ppe_cs_chomp */

/* -- Join & Concat -- */
 
PPE_API ppe_cstr ppe_cs_join(const ppe_cstr restrict d, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt, ...)
{
    ppe_cstr ret = NULL;
    va_list args;

    assert(d);
    assert(bsz);

    va_start(args, d);
    ret = ppe_cs_join_imp(d, ppe_cs_size(d), b, bsz, opt, ppe_true, &args);
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
    ret = ppe_cs_join_imp(cs_empty_s, 0, b, bsz, opt, ppe_false, &args);
    va_end(args);
    return ret;
}

/* -- Split & Slice -- */

PPE_API ppe_cs_snippet ppe_cs_split(const ppe_cstr restrict s, const ppe_cstr restrict d, ppe_cs_snippet restrict spt, ppe_uint * restrict n, const ppe_cstr * restrict ss, const ppe_str_option opt)
{
    if (! s || ! d || s == d) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (ppe_cs_is_empty(d)) {
        /* Cannot use a null or empty string as delimiter. */
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    return ppe_cs_split_imp(s, d, ppe_cs_size(d), spt, n, ss, opt);
} /* ppe_cs_split */

/* -- Replace & Substitute -- */

PPE_API ppe_cs_cstr ppe_cs_replace(const ppe_cstr restrict s, const ppe_size off, const ppe_size ssz, const ppe_cstr restrict to, ppe_cs_cstr restrict b, ppe_size * restrict bsz, const ppe_str_option opt)
{
    if (! s || ! to || to == s || b == s) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    return ppe_cs_replace_imp(s, ppe_cs_size(s), off, ssz, to, ppe_cs_size(to), b, bsz, opt);
} /* ppe_cs_replace */

PPE_API ppe_cs_cstr ppe_cs_substitute(const ppe_cstr restrict s, const ppe_cstr restrict from, const ppe_cstr restrict to, ppe_cs_cstr restrict b, ppe_size * restrict bsz, ppe_uint * restrict n, const ppe_str_option opt)
{
    if (! s || ! from || ! to || ppe_cs_is_empty(from)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (s == from || s == to || from == to) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    return ppe_cs_substitute_imp(s, from, frsz, to, tosz, b, bsz, n, opt);
} /* ppe_cs_substitute */

static ppe_cstr ppe_cs_sprintf_imp(ppe_cstr restrict b, ppe_size * restrict bsz, const ppe_cstr restrict fmt, va_list * restrict ap)
{
    int ret = 0;

    ret = vsnprintf(b, *bsz, fmt, *ap);
    if (ret < 0) {
        /* TODO: Error handling. */
        return NULL;
    }
    *bsz = ((ppe_size) ret); /* Excluding the terminating '\0' byte. */
    return b;
}

PPE_API ppe_cstr ppe_cs_sprintf(ppe_cstr restrict b, ppe_size * restrict bsz, const ppe_str_option opt, const ppe_cstr restrict fmt, ...)
{
    ppe_cstr ret = NULL;
    ppe_cstr nw = NULL;
    ppe_size nwsz = 128;
    ppe_size fmsz = 0;
    ppe_size obsz = *bsz;
    va_list args;

    if (b) {
        fmsz = obsz;

        va_start(args, fmt);
        ret = ppe_cs_sprintf_imp(b, &fmsz, fmt, &args);
        va_end(args);

        if (! ret) {
            return NULL;
        }

        if (obsz <= fmsz && (opt & PPE_STR_OPT_DONT_TRUNCATE)) {
            /* The output has been truncated but the caller want a fully formatted string. */
            ppe_err_set(PPE_ERR_OUT_OF_CAPACITY, NULL);
            return NULL;
        }

        *bsz = fmsz;
        return b;
    }

#if defined(PPE_CONF_SUPPORT_SUSV2)
    nw = (ppe_cstr) ppe_mp_malloc(nwsz);
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }
#endif

    /* Detect the number of characters which would have been written to the final string if enough space have been available. */
    fmsz = nwsz;

    va_start(args, fmt);
    ret = ppe_cs_sprintf_imp(nw, &fmsz, fmt, &args);
    va_end(args);

    if (! ret) {
#if defined(PPE_CONF_SUPPORT_SUSV2)
        ppe_mp_free(nw);
#endif
        return NULL;
    }

    if (opt & PPE_STR_OPT_NEW_STRING) {
        /* The caller want a new string. */

#if defined(PPE_CONF_SUPPORT_SUSV2)
        if (fmsz < nwsz) {
            ret = (ppe_cstr) ppe_mp_malloc(nw, fmsz + 1); /* Allocate a new buffer to avoid wasting memory. */
            if (ret) {
                /* Copy and replace the final string. */
                memcpy(ret, nw, fmsz + 1);
                ppe_mp_free(nw);
                nw = ret;
            }
            *bsz = fmsz; /* Excluding the terminating '\0' byte. */
            ppe_err_set(PPE_ERR_SUCCEED, NULL);
            return nw;
        }

        fmsz += 1; /* Including the terminating '\0' byte. */

        /* The final string would have been truncated. */
        ret = (ppe_cstr) ppe_mp_realloc(nw, fmsz); /* Expand the buffer for reformatting. */
        if (! ret) {
            ppe_mp_free(nw);
            ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
            return NULL;
        }
        nw = ret;
#else
        nw = (ppe_cstr) ppe_mp_malloc(fmsz);
        if (! nw) {
            ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
            return NULL;
        }
#endif

        /* Try to reformat again. */
        va_start(args, fmt);
        ret = ppe_cs_sprintf_imp(nw, &fmsz, fmt, &args); /* The terminating '\0' byte shall be written. */
        va_end(args);

        if (! ret) {
            ppe_mp_free(nw);
            return NULL;
        }

        b = nw;
    } /* if */

    *bsz = fmsz; /* Excluding the terminating '\0' byte. */
    ppe_err_set(PPE_ERR_SUCCEED, NULL);
    return b; /* Return Whatever the caller passed. */
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

PPE_API ppe_string ppe_str_create(const ppe_cstr restrict s, const ppe_size sz)
{
    ppe_string nw = NULL;

    if (! s) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (sz == 0 || ppe_cs_is_empty(s)) {
        return &str_empty_s;
    }

    nw = (ppe_string) ppe_mp_malloc(sizeof(ppe_string_st) + sz); /* Including the terminating NUL byte. */
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    memcpy(nw->buf, s, sz);
    nw->buf[sz] = '\0';
    nw->sz = sz;
    return nw;
} /* ppe_str_create */

PPE_API ppe_string ppe_str_clone(const ppe_string restrict s)
{
    return ppe_str_create(s->buf, s->sz);
} /* ppe_str_clone */

PPE_API void ppe_str_destroy(ppe_string restrict s)
{
    if (s && s != &str_empty_s) {
        ppe_mp_free(s);
    }
} /* ppe_str_destroy */

/* -- Manipulate -- */

static ppe_string ppe_str_trim_bytes_imp(const ppe_string restrict s, const ppe_cstr t, const ppe_str_option opt)
{
    const ppe_cstr p = NULL;
    const ppe_cstr q = NULL;
    ppe_size cpsz = 0;

    p = s->buf;
    if (opt & PPE_STR_OPT_DIRECT_LEFT) {
        while (*p && strchr(t, *p)) {
            p++;
        }
    } /* if */

    if (opt & PPE_STR_OPT_DIRECT_RIGHT) {
        q = s->buf + s->sz - 1;
        while (p <= q && strchr(t, *q)) {
            q--;
        }
        cpsz = q < p ? 0 : q - p + 1;
    } else {
        cpsz = s->sz - (p - s->buf);
    } /* if */
    return ppe_str_create(p, cpsz);
} /* ppe_str_trim_bytes_imp */

PPE_API ppe_string ppe_str_trim_bytes(const ppe_string restrict s, const ppe_string restrict t, const ppe_str_option opt)
{
    if (! s || ! t || s == t || ppe_str_is_empty(t)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (ppe_str_is_empty(s)) {
        return &str_empty_s;
    }
    return ppe_str_trim_bytes_imp(s, t->buf, opt);
} /* ppe_str_trim_bytes */

PPE_API ppe_string ppe_str_trim_bytes_cs(const ppe_string restrict s, const ppe_string restrict t, const ppe_str_option opt)
{
    if (! s || ! t || s->buf == t || ppe_cs_is_empty(t)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (ppe_str_is_empty(s)) {
        return &str_empty_s;
    }
    return ppe_str_trim_bytes_imp(s, t, opt);
} /* ppe_str_trim_bytes_cs */

PPE_API ppe_string ppe_str_chop(const ppe_string restrict s, const ppe_str_option opt)
{
    if (! s) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (ppe_cs_is_empty(s)) {
        return &str_empty_s;
    }
    return ppe_str_create(s->buf, s->sz - 1);
} /* ppe_str_chop */

PPE_API ppe_string ppe_str_chomp(const ppe_string restrict s, const ppe_str_option opt)
{
    ppe_cstr ret = NULL;
    ppe_string nw = NULL;
    ppe_size nwsz = 0;

    if (! s || ! t || s == t || ppe_cs_is_empty(t)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (ppe_str_is_empty(s)) {
        return &str_empty_s;
    }

    ppe_cs_chomp(s->buf, s->sz, NULL, &nwsz, opt);
    nw = (ppe_string) ppe_mp_malloc(sizeof(ppe_string_st) + nwsz - 1); /* The terminating NUL byte have been counted twice. */
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    ppe_cs_chomp(s->buf, s->sz, nw->buf, &nwsz, opt);
    nw->sz = nwsz;
    return nw;
} /* ppe_str_chomp */

/* -- Join & Concat -- */

static ppe_string ppe_str_join_imp(const ppe_string restrict d, const ppe_str_option opt, va_list * restrict args)
{
    ppe_cstr ret = NULL;
    ppe_string nw = NULL;
    ppe_size nwsz = 0;
    va_list cp;

    if (! d) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }

    va_copy(cp, *args);
    ret = ppe_cs_join_imp(d->buf, d->sz, NULL, &nwsz, opt, ppe_true, &cp);
    va_end(cp);
    if (! ret) {
        return NULL;
    }

    nw = (ppe_string) ppe_mp_malloc(sizeof(ppe_string_st) + nwsz - 1); /* The terminating NUL charater has been counted twice. */
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    va_copy(cp, *args);
    ret = ppe_cs_join_imp(d->buf, d->sz, nw->buf, &nwsz, opt, ppe_true, &cp); /* Include the terminating NUL character. */
    va_end(cp);
    if (! ret) {
        ppe_mp_free(nw);
        return NULL;
    }

    nw->sz = nwsz;
    return nw;
} /* ppe_str_join_imp */

PPE_API ppe_string ppe_str_join(const ppe_string restrict d, const ppe_str_option opt, ...)
{
    ppe_string ret = NULL;
    va_list args;

    if (! d) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }

    va_start(args, opt);
    ret = ppe_str_join_imp(d, &nwsz, opt, ppe_true, &args);
    va_end(args);
    return ret;
} /* ppe_str_join */

PPE_API ppe_string ppe_str_concat(const ppe_str_option opt, ...)
{
    ppe_string ret = NULL;
    va_list args;

    va_start(args, opt);
    ret = ppe_str_join_imp(&str_empty_s, NULL, &nwsz, opt, ppe_true, &args);
    va_end(args);
    return ret;
} /* ppe_str_concat */

/* -- Split -- */

PPE_API ppe_cs_snippet ppe_str_split(const ppe_string restrict s, const ppe_string restrict d, ppe_cs_snippet restrict spt, const const ppe_uint * restrict n, const ppe_str_option opt)
{
    if (! s || ! d || s == d) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (ppe_str_is_empty(d)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    return ppe_cs_split_imp(s->buf, d->buf, d->sz, spt, n, NULL, opt);
}

PPE_API ppe_cs_snippet ppe_str_split_cs(const ppe_string restrict s, const ppe_cstr restrict d, ppe_cs_snippet restrict spt, const const ppe_uint * restrict n, const ppe_str_option opt)
{
    if (! s || ! d) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (ppe_cs_is_empty(d)) {
        /* Cannot use a null or empty string as delimiter. */
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    return ppe_cs_split_imp(s->buf, d, ppe_cs_size(d), spt, n, NULL, opt);
}

/* -- Replace & Substitute -- */

PPE_API ppe_string ppe_str_replace(const ppe_string restrict s, const ppe_size off, const ppe_size ssz, const ppe_string restrict to, const ppe_str_option opt)
{
    ppe_cstr ret = NULL;
    ppe_string nw = NULL;
    ppe_size nwsz = 0;

    if (! s || ! to || to == s) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }

    ret = ppe_cs_replace_imp(s->buf, s->sz, off, ssz, to->buf, to->sz, NULL, &nwsz, opt);
    if (! ret && ppe_err_get_code() != PPE_ERR_CALL_AGAIN) {
        return NULL;
    }

    if (nwsz == 1) {
        return &str_empty_s;
    }

    nw = (ppe_string) malloc(sizeof(ppe_string_st) + nwsz - 1); /* The terminating NUL byte has been counted twice. */
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    ret = ppe_cs_replace_imp(s->buf, s->sz, off, ssz, to->buf, to->sz, nw->buf, &nwsz, opt);
    if (! ret) {
        ppe_mp_free(nw);
        return NULL;
    }
    nw->sz = nwsz;
    return nw;
} /* ppe_str_replace */

PPE_API ppe_string ppe_str_replace_cs(const ppe_string restrict s, const ppe_size off, const ppe_size ssz, const ppe_cstr restrict to, const ppe_str_option opt)
{
    ppe_cstr ret = NULL;
    ppe_string nw = NULL;
    ppe_size nwsz = 0;
    ppe_size tosz = 0;

    if (! s || ! to) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }

    tosz = ppe_cs_size(to);

    ret = ppe_cs_replace_imp(s->buf, s->sz, off, ssz, to, tosz, NULL, &nwsz, opt);
    if (! ret && ppe_err_get_code() != PPE_ERR_CALL_AGAIN) {
        return NULL;
    }

    if (nwsz == 1) {
        return &str_empty_s;
    }

    nw = (ppe_string) malloc(sizeof(ppe_string_st) + nwsz - 1); /* The terminating NUL byte has been counted twice. */
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    ret = ppe_cs_replace_imp(s->buf, s->sz, off, ssz, to, tosz, nw->buf, &nwsz, opt);
    if (! ret) {
        ppe_mp_free(nw);
        return NULL;
    }
    nw->sz = nwsz;
    return nw;
} /* ppe_str_replace_cs */

PPE_API ppe_string ppe_str_substitute(const ppe_string restrict s, const ppe_string restrict from, const ppe_string restrict to, ppe_uint * restrict n, const ppe_str_option opt)
{
    ppe_cstr ret = NULL;
    ppe_string nw = NULL;
    ppe_size nwsz = 0;

    if (! s || ! from || ! to || s == from || s == to || from == to) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (ppe_str_is_empty(from)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }

    ret = ppe_cs_substitute_imp(s->buf, from->buf, from->sz, to->buf, to->sz, NULL, &nwsz, n, opt);
    if (! ret && ppe_err_get_code() != PPE_ERR_CALL_AGAIN) {
        return NULL;
    }

    if (nwsz == 1) {
        return &str_empty_s;
    }

    nw = (ppe_string) malloc(sizeof(ppe_string_st) + nwsz - 1); /* The terminating NUL byte has been counted twice. */
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    ret = ppe_cs_substitute_imp(s->buf, from->buf, from->sz, to->buf, to->sz, nw->buf, &nwsz, n, opt);
    if (! ret) {
        ppe_mp_free(nw);
        return NULL;
    }
    nw->sz = nwsz;
    return nw;
} /* ppe_str_substitute */

PPE_API ppe_string ppe_str_substitute_cs(const ppe_string restrict s, const ppe_cstr restrict from, const ppe_cstr restrict to, ppe_uint * restrict n, const ppe_str_option opt)
{
    ppe_cstr ret = NULL;
    ppe_string nw = NULL;
    ppe_size nwsz = 0;
    ppe_size frsz = 0;
    ppe_size tosz = 0;

    if (! s || ! from || ! to || from == to) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (ppe_cs_is_empty(from)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }

    frsz = ppe_cs_size(from);
    tosz = ppe_cs_size(to);

    ret = ppe_cs_substitute_imp(s->buf, from, frsz, to, tosz, NULL, &nwsz, n, opt);
    if (! ret && ppe_err_get_code() != PPE_ERR_CALL_AGAIN) {
        return NULL;
    }

    if (nwsz == 1) {
        return &str_empty_s;
    }

    nw = (ppe_string) malloc(sizeof(ppe_string_st) + nwsz - 1); /* The terminating NUL byte has been counted twice. */
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    ret = ppe_cs_substitute_imp(s->buf, from, frsz, to, tosz, nw->buf, &nwsz, n, opt);
    if (! ret) {
        ppe_mp_free(nw);
        return NULL;
    }
    nw->sz = nwsz;
    return nw;
} /* ppe_str_substitute_cs */

#ifdef __cplusplus
}
#endif

