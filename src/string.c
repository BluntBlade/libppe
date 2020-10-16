#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "ppe/mempool.h"
#include "ppe/string.h"
#include "ppe/error.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ==== Declaration : C-String Snippet ====================================== */

/* ---- Types --------------------------------------------------------------- */

typedef struct PPE_CSPT_ITEM {
    ppe_cstr_c s;
    ppe_size sz;
} ppe_cspt_item_st;

typedef struct PPE_CS_SNIPPET {
    ppe_int cnt;
    ppe_int cap;
    ppe_size total;
    ppe_cspt_item_st items[1];
} ppe_cs_snippet_st;

/* ---- Preset Values ------------------------------------------------------- */

static const ppe_cstr_c cs_empty_s = "";
static ppe_cs_snippet_st cspt_empty_s = {1, 1, 0, {{cs_empty_s, 0}}};

/* -- Property -- */

PPE_API ppe_int ppe_cspt_count(const ppe_cs_snippet restrict spt)
{
    return spt->cnt;
}

PPE_API ppe_int ppe_cspt_capacity(const ppe_cs_snippet restrict spt)
{
    return spt->cap;
}

PPE_API ppe_size ppe_cspt_total_size(const ppe_cs_snippet restrict spt)
{
    return spt->total;
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
    nw->total = 0;
    return nw;
} /* ppe_cspt_create */

PPE_API void ppe_cspt_destroy(ppe_cs_snippet restrict spt)
{
    if (spt && spt != &cspt_empty_s) {
        ppe_mp_free(spt);
    }
} /* ppe_cspt_destroy */

PPE_API void ppe_cspt_reset(ppe_cs_snippet restrict spt)
{
    spt->cnt = 0;
    spt->total = 0;
} /* ppe_cspt_reset */

/* -- Manipulators -- */

PPE_API ppe_bool ppe_cspt_get(ppe_cs_snippet const restrict spt, const ppe_uint idx, ppe_cstr_c * const restrict s, ppe_size * restrict sz)
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
} /* ppe_cspt_get */

PPE_API ppe_bool ppe_cspt_append(ppe_cs_snippet restrict spt, ppe_cstr_c const restrict s, const ppe_size sz)
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
    spt->total += sz;
    return ppe_true;
} /* ppe_cspt_append */

/* -- Producer -- */

PPE_API ppe_cstr_c ppe_cspt_copy(ppe_cs_snippet restrict spt, const ppe_uint idx, ppe_cstr restrict b, ppe_size * restrict bsz)
{
    ppe_cstr_c s = NULL;
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
            ppe_err_set(PPE_ERR_TRY_AGAIN, NULL);
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
} /* ppe_cspt_copy */

PPE_API ppe_cstr_c * ppe_cspt_copy_some(const ppe_cs_snippet restrict spt, const ppe_uint idx, ppe_uint * restrict n)
{
    ppe_cstr_c * nw = NULL;
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

    nw = (ppe_cstr_c *) ppe_mp_malloc(sizeof(ppe_cstr) * (max + 1));
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
} /* ppe_cspt_copy_some */

/* ==== Definitions : C-String ============================================== */

/* ---- Functions ----------------------------------------------------------- */

/* -- Internals -- */

#define PPE_CONF_STR_SNIPPET_MAX 64

static ppe_cs_snippet cs_split(ppe_cstr_c const restrict s, ppe_cstr_c const restrict d, const ppe_size dsz, ppe_cs_snippet restrict spt, ppe_uint * restrict n, ppe_cstr_c * restrict ss, const ppe_str_option opt)
{
    ppe_cs_snippet nw = NULL;
    ppe_cstr_c p = 0;
    ppe_cstr_c q = 0;
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
            return &cspt_empty_s;
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
} /* cs_split */

static ppe_cstr_c cs_replace(ppe_cstr_c const restrict s, const ppe_size sz, const ppe_size off, ppe_size ssz, ppe_cstr_c const restrict to, const ppe_size tosz, ppe_cstr restrict b, ppe_size * restrict bsz, const ppe_str_option opt)
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
} /* cs_replace */

#define PPE_CONF_STR_SUBSTITUTE_MAX 256

static ppe_cstr_c cs_substitute(ppe_cstr_c const restrict s, ppe_cstr_c const restrict from, const ppe_size frsz, ppe_cstr_c const to, const ppe_size tosz, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_uint * restrict n, const ppe_str_option opt)
{
    ppe_cstr_c p = NULL;
    ppe_cstr_c q = NULL;
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
            ppe_err_set(PPE_ERR_TRY_AGAIN, NULL);
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
} /* cs_substitute */

/* -- Preset values -- */

PPE_API ppe_cstr_c ppe_cs_get_empty(void)
{
    return cs_empty_s;
}

/* -- Property -- */

PPE_API ppe_size ppe_cs_size(ppe_cstr_c const restrict s)
{
    assert(s);
    return strlen(s);
}

/* -- Test -- */

PPE_API ppe_bool ppe_cs_is_empty(ppe_cstr_c const restrict s)
{
    assert(s);
    return (s[0] == '\0');
}

PPE_API ppe_int ppe_cs_compare(ppe_cstr_c const restrict s1, ppe_cstr_c const restrict s2)
{
    assert(s1);
    assert(s2);
    return strcmp(s1, s2);
}

/* -- Find -- */

PPE_API ppe_cstr_c const ppe_cs_find(ppe_cstr_c const restrict s, ppe_cstr_c const restrict t)
{
    assert(s);
    assert(t);
    return (const ppe_cstr) strstr(s, t);
}

/* -- Create & Destroy -- */

PPE_API void ppe_cs_destroy(ppe_cstr_c restrict s)
{
    if (s && s != cs_empty_s) {
        ppe_mp_free((void *)s);
    }
}

PPE_API ppe_cstr_c ppe_cs_create(ppe_cstr_c const restrict s, const ppe_size sz)
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

PPE_API ppe_cstr_c ppe_cs_substr(ppe_cstr_c const restrict s, const ppe_size off, const ppe_size ssz, ppe_cstr restrict b, ppe_size * bsz, ppe_str_option opt)
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
            ppe_err_set(PPE_ERR_TRY_AGAIN, NULL);
            return NULL;
        }
        
        /* NEW-STRING MODE */
        return ppe_cs_create(s + off, cpsz);
    } /* if */

    /* FILL-BUFFER MODE */
    if (! bsz) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (*bsz < cpsz + 1) {
        if (opt & PPE_STR_OPT_DONT_TRUNCATE) {
            ppe_err_set(PPE_ERR_OUT_OF_CAPACITY, NULL);
            return NULL;
        }
        cpsz = *bsz - 1;
    } /* if */

    memcpy(b, s + off, cpsz);
    b[cpsz] = '\0';
    *bsz = cpsz;
    return b;
} /* ppe_cs_substr */

/* -- Trim & Chomp -- */

static ppe_cstr_c cs_dupstr(ppe_cstr_c const restrict s, const ppe_size cpsz, ppe_cstr restrict b, ppe_size * restrict bsz)
{
    if (! b) {
        if (bsz) {
            /* MEASURE-SIZE MODE */
            *bsz = cpsz + 1; /* Include the terminating NUL byte. */
            ppe_err_set(PPE_ERR_TRY_AGAIN, NULL);
            return NULL;
        }
        /* NEW-STRING MODE */
        return cpsz > 0 ? ppe_cs_create(s, cpsz) : cs_empty_s;
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
} /* cs_dupstr */

static ppe_cstr_c cs_get_trim_position(ppe_cstr_c restrict p, ppe_cstr_c restrict q, ppe_cstr_c restrict t, ppe_size * restrict sz, const ppe_str_option opt)
{
    assert(p <= q);
    if (opt & PPE_STR_OPT_LEFT_END) {
        while (*p && strchr(t, *p)) { ++p; }
    } /* if */
    if (opt & PPE_STR_OPT_RIGHT_END) {
        while (p <= q && strchr(t, *q)) { --q; }
    } /* if */
    if (q < p) {
        *sz = 0;
        return cs_empty_s;
    }
    *sz = q - p + 1;
    return p;
} /* cs_get_trim_position */

PPE_API ppe_cstr_c ppe_cs_trim(ppe_cstr_c const restrict s, ppe_cstr_c const restrict t, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt)
{
    ppe_cstr_c p = NULL;
    ppe_size cpsz = 0;

    if (! s || ! t || s == t || ppe_cs_is_empty(t)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }

    p = cs_get_trim_position(s, s + ppe_cs_size(s) - 1, t, &cpsz, opt);
    return cs_dupstr(p, cpsz, b, bsz);
} /* ppe_cs_trim */

PPE_API ppe_cstr_c ppe_cs_chop(ppe_cstr_c const restrict s, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt)
{
    ppe_size cpsz = 0;

    if (! s) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }

    if (ppe_cs_is_empty(s)) {
        cpsz = 0;
    } else {
        /* TODO: What if chopping a UTF-8 character? */
        cpsz = ppe_cs_size(s) - 1;
    } /* if */
    return cs_dupstr(s, cpsz, b, bsz);
} /* ppe_cs_chop */

static ppe_cstr_c cs_chomp(ppe_cstr_c const restrict s, const ppe_size sz, ppe_cstr_c const restrict t, const ppe_size tsz, const ppe_int n, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt)
{
    ppe_size cpsz = sz;
    ppe_int i = n;

    if (ppe_cs_is_empty(s)) {
        cpsz = 0;
    } else {
        do {
            if (strncmp(s + cpsz - tsz, t, tsz) == 0) {
                cpsz -= tsz;
            } else {
                break;
            } /* if */
        } while ((n < 0 || --i > 0) && cpsz >= tsz);
    } /* if */
    return cs_dupstr(s, cpsz, b, bsz);
} /* cs_chomp */

PPE_API ppe_cstr_c ppe_cs_chomp(ppe_cstr_c const restrict s, ppe_cstr_c const restrict t, const ppe_int n, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt)
{
    if (! s || n == 0) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    return cs_chomp(s, ppe_cs_size(s), t, ppe_cs_size(t), n, b, bsz, opt);
} /* ppe_cs_chomp */

/* -- Join & Concat -- */

static ppe_cstr_c cs_join_s(ppe_cstr_c restrict d, ppe_size dsz, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt, va_list * restrict ap)
{
    va_list cp;
    ppe_bool ret = ppe_false;
    ppe_cstr s = NULL;
    ppe_size sz = 0;
    ppe_cstr nw = NULL;
    ppe_sjn_joiner jnr = NULL;
    ppe_str_action act = PPE_STR_JOIN_END;

    jnr = ppe_sjn_create(d, dsz);
    if (! jnr) {
        return NULL;
    }

    /* -- Check all arugments and measure the total bytes needed to hold the result. -- */
    va_copy(cp, *ap);
    while ((act = va_arg(cp, ppe_str_action)) != PPE_STR_JOIN_END) {
        switch (act) {
            case PPE_STR_JOIN_CSTR:
                s = va_arg(cp, ppe_cstr);
                if (! s) {
                    ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
                    goto CS_JOIN_S_ERROR;
                }
                if (ppe_sjn_append_cstr(jnr, PPE_SJN_MEASURE, s, ppe_cs_size(s), NULL, NULL, NULL) < 0) {
                    goto CS_JOIN_S_ERROR;
                } /* if */
                break;

            case PPE_STR_JOIN_CSTR_WITH_SIZE:
                break;

            case PPE_STR_JOIN_CSTR_ARRAY:
                break;

            case PPE_STR_JOIN_STRING:
                break;

            case PPE_STR_JOIN_STRING_ARRAY:
                break;

            case PPE_STR_JOIN_SNIPPET:
                break;

            default:
                ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
                return NULL;
        } /* switch */
    } /* while */
    va_end(cp);

    if (! ret) {
        return NULL;
    }

    if (b) {
        if (! bsz) {
            ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
            return NULL;
        }
        if (*bsz < args.cpsz + 1) {
            ppe_err_set(PPE_ERR_OUT_OF_CAPACITY, NULL);
            return NULL;
        }

        if ((args.cpsz - dsz) == 0) {
            b[0] = '\0';
            return b;
        }

        args.b = b;
    } else {
        if (bsz) {
            /* MEASURE-SIZE MODE */
            *bsz = args.cpsz + 1; /* Include the terminating NUL character. */
            ppe_err_set(PPE_ERR_TRY_AGAIN, NULL);
            return NULL;
        }

        /* NEW-STRING MODE */
        if ((args.cpsz - dsz) == 0) {
            return cs_empty_s;
        }

        nw = (ppe_cstr) ppe_mp_malloc(args.cpsz + 1); /* Include the terminating NUL character. */
        if (! nw) {
            ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
            return NULL;
        }

        args.b = nw;
    } /* if */

    args.cpsz = 0;

    va_copy(cp, *ap);
    args.cp = &cp;
    cs_join_act(copy_s, &args);
    va_end(cp);

    args.cpsz -= dsz;

    if (! bsz) {
        /* NEW-STRING MODE */
        nw[args.cpsz] = '\0';
    } else {
        /* FILL-BUFFER MODE */
        if (args.cpsz + 1 <= *bsz) {
            nw[args.cpsz] = '\0';
        }
        *bsz = args.cpsz;
    } /* if */
    return (ppe_cstr_c) nw;

CS_JOIN_S_ERROR;
    ppe_sjn_destroy(jnr);
    return NULL;
} /* cs_join_s */
 
PPE_API ppe_cstr_c ppe_cs_join(ppe_cstr_c const restrict d, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt, ...)
{
    ppe_cstr_c ret = NULL;
    va_list args;

    va_start(args, opt);
    ret = cs_join(d, ppe_cs_size(d), b, bsz, opt, &args);
    va_end(args);
    return ret;
} /* ppe_cs_join */
 
PPE_API ppe_cstr_c ppe_cs_concat(ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt, ...)
{
    ppe_cstr_c ret = NULL;
    va_list args;

    va_start(args, opt);
    ret = cs_join(cs_empty_s, 0, b, bsz, opt, &args);
    va_end(args);
    return ret;
} /* ppe_cs_concat */

/* -- Split & Slice -- */

PPE_API ppe_cs_snippet ppe_cs_split(const ppe_cstr restrict s, const ppe_cstr restrict d, ppe_cs_snippet restrict spt, ppe_uint * restrict n, ppe_cstr_c * restrict ss, const ppe_str_option opt)
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
    return cs_split(s, d, ppe_cs_size(d), spt, n, ss, opt);
} /* ppe_cs_split */

/* -- Replace & Substitute -- */

PPE_API ppe_cstr_c ppe_cs_replace(ppe_cstr_c const restrict s, const ppe_size off, const ppe_size ssz, ppe_cstr_c const restrict to, ppe_cstr restrict b, ppe_size * restrict bsz, const ppe_str_option opt)
{
    if (! s || ! to || to == s || b == s) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    return cs_replace(s, ppe_cs_size(s), off, ssz, to, ppe_cs_size(to), b, bsz, opt);
} /* ppe_cs_replace */

PPE_API ppe_cstr_c ppe_cs_substitute(ppe_cstr_c const restrict s, ppe_cstr_c const restrict from, ppe_cstr_c const restrict to, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_uint * restrict n, const ppe_str_option opt)
{
    if (! s || ! from || ! to || ppe_cs_is_empty(from)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (s == from || s == to || from == to) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    return cs_substitute(s, from, ppe_cs_size(from), to, ppe_cs_size(to), b, bsz, n, opt);
} /* ppe_cs_substitute */

static ppe_cstr cs_sprintf(ppe_cstr restrict b, ppe_size * restrict bsz, const ppe_cstr restrict fmt, va_list * restrict ap)
{
    int ret = 0;

    ret = vsnprintf(b, *bsz, fmt, *ap);
    if (ret < 0) {
        /* TODO: Error handling. */
        return NULL;
    }
    *bsz = ((ppe_size) ret); /* Excluding the terminating '\0' byte. */
    return b;
} /* cs_sprintf */

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
        ret = cs_sprintf(b, &fmsz, fmt, &args);
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
    ret = cs_sprintf(nw, &fmsz, fmt, &args);
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
        ret = cs_sprintf(nw, &fmsz, fmt, &args); /* The terminating '\0' byte shall be written. */
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

static ppe_string_st str_empty_s = { 0, {'\0'}};

/* ---- Functions ----------------------------------------------------------- */

/* -- Internals -- */

static inline void ppe_str_get_var_args_with_size(const ppe_str_option opt, const void * restrict v, const ppe_size vsz, ppe_cstr_c * restrict t, ppe_size * tsz)
{
    if (opt & PPE_STR_OPT_CSTR_ARG) {
        *t = (ppe_cstr_c) v;
        *tsz = vsz;
    } else {
        *t = ((const ppe_string) v)->buf;
        *tsz = ((const ppe_string) v)->sz;
    } /* if */
} /* ppe_str_get_var_args_with_size */

static inline void ppe_str_get_var_args(const ppe_str_option opt, const void * restrict v, const ppe_size vsz, ppe_cstr_c * restrict t)
{
    if (opt & PPE_STR_OPT_CSTR_ARG) {
        *t = (const ppe_cstr) v;
    } else {
        *t = ((const ppe_string) v)->buf;
    } /* if */
} /* ppe_str_get_var_args */

/* -- Preset values -- */

PPE_API const ppe_string ppe_str_get_empty(void)
{
    return &str_empty_s;
}

/* -- Property -- */

PPE_API ppe_cstr_c ppe_str_addr(ppe_string restrict s)
{
    assert(s != NULL);
    return s->buf;
}

PPE_API ppe_ssize ppe_str_size(ppe_string restrict s)
{
    assert(s != NULL);
    return s->sz;
}

/* -- Test -- */

PPE_API ppe_bool ppe_str_is_empty(ppe_string restrict s)
{
    return (s->sz == 0);
} /* ppe_str_is_empty */

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

PPE_API ppe_string ppe_str_create(ppe_cstr_c const restrict s, const ppe_size sz)
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

PPE_API ppe_string ppe_str_clone(ppe_string const restrict s)
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

PPE_API ppe_string ppe_str_substr(const ppe_string restrict s, const ppe_size off, const ppe_size rsz, const ppe_str_option opt)
{
    if (! s) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (s->sz <= off) {
        return &str_empty_s;
    }
    return ppe_str_create(s->buf + off, (s->sz - off < rsz) ? (s->sz - off) : rsz);
} /* ppe_str_substr */

PPE_API ppe_string ppe_str_trim_ex(const ppe_string restrict s, const ppe_str_option opt, const void * restrict v, const ppe_size vsz)
{
    ppe_cstr_c t = NULL;
    ppe_cstr_c p = NULL;
    ppe_size cpsz = 0;

    if (! s) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    } else if (ppe_str_is_empty(s)) {
        return &str_empty_s;
    } /* if */

    ppe_str_get_var_args(opt, v, vsz, &t);
    if (! t) {
        t = PPE_STR_SPACES;
    } else if (ppe_cs_is_empty(t) || s->buf == t) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    } /* if */

    p = cs_get_trim_position(s->buf, s->buf + s->sz - 1, t, &cpsz, opt);
    return ppe_str_create(p, cpsz);
} /* ppe_str_trim_ex */

PPE_API ppe_string ppe_str_chop(const ppe_string restrict s, const ppe_str_option opt)
{
    if (! s) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (ppe_str_is_empty(s)) {
        return &str_empty_s;
    }
    return ppe_str_create(s->buf, s->sz - 1);
} /* ppe_str_chop */

PPE_API ppe_string ppe_str_chomp_ex(const ppe_string restrict s, const ppe_str_option opt, const void * restrict v, const ppe_size vsz)
{
    ppe_string nw = NULL;
    ppe_cstr_c t = NULL;
    ppe_size tsz = 0;
    ppe_size nwsz = 0;

    ppe_str_get_var_args_with_size(opt, v, vsz, &t, &tsz);

    if (! s) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    } else if (ppe_str_is_empty(s)) {
        return &str_empty_s;
    } /* if */

    if (! t) {
        t = PPE_STR_NEWLINE;
        tsz = strlen(PPE_STR_NEWLINE);
    } else if (tsz == 0 || ppe_cs_is_empty(t) || s->buf == t) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    } /* if */

    cs_chomp(s->buf, s->sz, PPE_STR_NEWLINE, strlen(PPE_STR_NEWLINE), PPE_STR_UNLIMITED, NULL, &nwsz, opt);
    nw = (ppe_string) ppe_mp_malloc(sizeof(ppe_string_st) + nwsz - 1); /* The terminating NUL byte have been counted twice. */
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    cs_chomp(s->buf, s->sz, PPE_STR_NEWLINE, strlen(PPE_STR_NEWLINE), PPE_STR_UNLIMITED, nw->buf, &nwsz, opt);
    nw->sz = nwsz;
    return nw;
} /* ppe_str_chomp */

/* -- Join & Concat -- */

static ppe_string ppe_str_join_imp(const ppe_string restrict d, const ppe_str_option opt, va_list * restrict args)
{
    ppe_cstr_c ret = NULL;
    ppe_string nw = NULL;
    ppe_size nwsz = 0;
    va_list cp;

    if (! d) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }

    va_copy(cp, *args);
    ret = cs_join(d->buf, d->sz, NULL, &nwsz, opt, &cp);
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
    ret = cs_join(d->buf, d->sz, nw->buf, &nwsz, opt, &cp); /* Include the terminating NUL character. */
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
    ret = ppe_str_join_imp(d, opt, &args);
    va_end(args);
    return ret;
} /* ppe_str_join */

PPE_API ppe_string ppe_str_concat(const ppe_str_option opt, ...)
{
    ppe_string ret = NULL;
    va_list args;

    va_start(args, opt);
    ret = ppe_str_join_imp(&str_empty_s, opt, &args);
    va_end(args);
    return ret;
} /* ppe_str_concat */

/* -- Split -- */

PPE_API ppe_cs_snippet ppe_str_split(const ppe_string restrict s, const ppe_string restrict d, ppe_cs_snippet restrict spt, ppe_uint * restrict n, const ppe_str_option opt)
{
    if (! s || ! d || s == d) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (ppe_str_is_empty(d)) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    return cs_split(s->buf, d->buf, d->sz, spt, n, NULL, opt);
}

PPE_API ppe_cs_snippet ppe_str_split_cs(const ppe_string restrict s, const ppe_cstr restrict d, ppe_cs_snippet restrict spt, ppe_uint * restrict n, const ppe_str_option opt)
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
    return cs_split(s->buf, d, ppe_cs_size(d), spt, n, NULL, opt);
}

/* -- Replace & Substitute -- */

PPE_API ppe_string ppe_str_replace(const ppe_string restrict s, const ppe_size off, const ppe_size ssz, const ppe_string restrict to, const ppe_str_option opt)
{
    ppe_cstr_c ret = NULL;
    ppe_string nw = NULL;
    ppe_size nwsz = 0;

    if (! s || ! to || to == s) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }

    ret = cs_replace(s->buf, s->sz, off, ssz, to->buf, to->sz, NULL, &nwsz, opt);
    if (! ret && ppe_err_get_code() != PPE_ERR_TRY_AGAIN) {
        return NULL;
    }

    if (nwsz == 1) {
        return &str_empty_s;
    }

    nw = (ppe_string) ppe_mp_malloc(sizeof(ppe_string_st) + nwsz - 1); /* The terminating NUL byte has been counted twice. */
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    ret = cs_replace(s->buf, s->sz, off, ssz, to->buf, to->sz, nw->buf, &nwsz, opt);
    if (! ret) {
        ppe_mp_free(nw);
        return NULL;
    }
    nw->sz = nwsz;
    return nw;
} /* ppe_str_replace */

PPE_API ppe_string ppe_str_replace_cs(const ppe_string restrict s, const ppe_size off, const ppe_size ssz, const ppe_cstr restrict to, const ppe_str_option opt)
{
    ppe_cstr_c ret = NULL;
    ppe_string nw = NULL;
    ppe_size nwsz = 0;
    ppe_size tosz = 0;

    if (! s || ! to) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }

    tosz = ppe_cs_size(to);

    ret = cs_replace(s->buf, s->sz, off, ssz, to, tosz, NULL, &nwsz, opt);
    if (! ret && ppe_err_get_code() != PPE_ERR_TRY_AGAIN) {
        return NULL;
    }

    if (nwsz == 1) {
        return &str_empty_s;
    }

    nw = (ppe_string) ppe_mp_malloc(sizeof(ppe_string_st) + nwsz - 1); /* The terminating NUL byte has been counted twice. */
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    ret = cs_replace(s->buf, s->sz, off, ssz, to, tosz, nw->buf, &nwsz, opt);
    if (! ret) {
        ppe_mp_free(nw);
        return NULL;
    }
    nw->sz = nwsz;
    return nw;
} /* ppe_str_replace_cs */

PPE_API ppe_string ppe_str_substitute(const ppe_string restrict s, const ppe_string restrict from, const ppe_string restrict to, ppe_uint * restrict n, const ppe_str_option opt)
{
    ppe_cstr_c ret = NULL;
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

    ret = cs_substitute(s->buf, from->buf, from->sz, to->buf, to->sz, NULL, &nwsz, n, opt);
    if (! ret && ppe_err_get_code() != PPE_ERR_TRY_AGAIN) {
        return NULL;
    }

    if (nwsz == 1) {
        return &str_empty_s;
    }

    nw = (ppe_string) ppe_mp_malloc(sizeof(ppe_string_st) + nwsz - 1); /* The terminating NUL byte has been counted twice. */
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    ret = cs_substitute(s->buf, from->buf, from->sz, to->buf, to->sz, nw->buf, &nwsz, n, opt);
    if (! ret) {
        ppe_mp_free(nw);
        return NULL;
    }
    nw->sz = nwsz;
    return nw;
} /* ppe_str_substitute */

PPE_API ppe_string ppe_str_substitute_cs(const ppe_string restrict s, const ppe_cstr restrict from, const ppe_cstr restrict to, ppe_uint * restrict n, const ppe_str_option opt)
{
    ppe_cstr_c ret = NULL;
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

    ret = cs_substitute(s->buf, from, frsz, to, tosz, NULL, &nwsz, n, opt);
    if (! ret && ppe_err_get_code() != PPE_ERR_TRY_AGAIN) {
        return NULL;
    }

    if (nwsz == 1) {
        return &str_empty_s;
    }

    nw = (ppe_string) ppe_mp_malloc(sizeof(ppe_string_st) + nwsz - 1); /* The terminating NUL byte has been counted twice. */
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    ret = cs_substitute(s->buf, from, frsz, to, tosz, nw->buf, &nwsz, n, opt);
    if (! ret) {
        ppe_mp_free(nw);
        return NULL;
    }
    nw->sz = nwsz;
    return nw;
} /* ppe_str_substitute_cs */

/* ==== Definition : String Joiner ========================================= */

/* ---- Types --------------------------------------------------------------- */

typedef struct PPE_SJN_JOINER
{
    ppe_uint i;     /* The index of the next string to add. */
    ppe_uint n;     /* Total number of added strings. */
    ppe_size sz;    /* Total size of appended strings and delimiters. */
    ppe_size dsz;   /* The length of the delimiter. */
    ppe_char d[1];  /* The delimiter. */
} ppe_sjn_joiner_st;

/* ---- Functions ----------------------------------------------------------- */

/* -- Property -- */

PPE_API ppe_uint ppe_sjn_count(ppe_sjn_joiner_c restrict jnr)
{
    return jnr->n;
} /* ppe_sjn_count */

PPE_API ppe_size ppe_sjn_size(ppe_sjn_joiner_c restrict jnr)
{
    return jnr->sz;
} /* ppe_sjn_size */

/* -- Create & Destroy -- */

PPE_API ppe_sjn_joiner ppe_sjn_create(ppe_cstr restrict d, const ppe_size dsz)
{
    ppe_sjn_joiner nw = NULL;

    nw = ppe_mp_calloc(sizeof(ppe_sjn_joiner_st) + dsz);
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    memcpy(nw->d, d, dsz);
    nw->dsz = dsz;
    return nw;
} /* ppe_sjn_create */

PPE_API void ppe_sjn_destroy(ppe_sjn_joiner restrict jnr)
{
    ppe_mp_free(jnr);
} /* ppe_sjn_destroy */

PPE_API void ppe_sjn_reset(ppe_sjn_joiner restrict jnr)
{
    jnr->i = 0;
    jnr->n = 0;
    jnr->sz = 0;
} /* ppe_sjn_reset */

/* -- Process -- */

static ppe_bool ppe_sjn_measure_imp(ppe_sjn_joiner sjn, const ppe_char * s, const ppe_size sz)
{
    if (jnr->n > 0) {
        jnr->sz += jnr->dsz;
    }
    jnr->sz += sz;
    jnr->n += 1;
    return ppe_true;
} /* ppe_sjn_join_imp */

static ppe_bool ppe_sjn_join_imp(ppe_sjn_joiner sjn, const ppe_char * s, const ppe_size sz)
{
    if (jnr->n > 0) {
        if (jnr->bsz - jnr->cpsz < jnr->dsz + sz) {
            jnr->nbsz = jnr->dsz + sz;
            ppe_err_set(PPE_ERR_OUT_OF_BUFFER, NULL);
            return ppe_false;
        }
        memcpy(jnr->buf + jnr->cpsz, jnr->d, jnr->dsz);
        jnr->cpsz += dsz;
        jnr->sz += dsz;
    } else if (jnr->bsz - jnr->cpsz < sz) {
        jnr->nbsz = sz;
        ppe_err_set(PPE_ERR_OUT_OF_BUFFER, NULL);
        return ppe_false;
    }
    memcpy(jnr->buf + jnr->cpsz, s, sz);
    jnr->cpsz += sz;
    jnr->sz += sz;
    jnr->i += 1;
    jnr->n += 1;
    return ppe_true;
} /* ppe_sjn_join_imp */

PPE_API ppe_int ppe_sjn_measure(ppe_sjn_joiner restrict jnr, void * restrict ud, ppe_sjn_yield_fn y, ppe_size * restrict nbsz)
{
    const ppe_char * s = NULL;
    const ppe_size sz = 0;
    ppe_int ret = 0;

    if (! jnr || ! y || ! nbsz) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return -1;
    }

    if (jnr->n == 0) {
        ret = (*y)(ud, jnr->i, &s, &sz);
        if (ret < 0) {
            return ret;
        } else if (ret == 0) {
            goto PPE_SJN_MEASURE_END;
        } /* if */
        goto PPE_SJN_MEASURE_NEXT;
    } /* if */

    do {
        jnr->sz += jnr->dsz;

PPE_SJN_MEASURE_NEXT:
        jnr->sz += sz;
        jnr->i += 1;
        jnr->n += 1;
    } while ((ret = (*y)(ud, jnr->i, &s, &sz)) > 0);

PPE_SJN_MEASURE_END:
    if (ret == 0) {
        jnr->i = 0;
    }
    *nbsz = jnr->sz > *bsz ? jnr->sz : *bsz;
    return ret;
} /* ppe_sjn_measure */

PPE_API ppe_int ppe_sjn_join(ppe_sjn_joiner restrict jnr, void * restrict ud, ppe_sjn_yield_fn y, ppe_char * restrict b, ppe_size * restrict bsz, ppe_size * restrict nbsz)
{
    const ppe_char * s = NULL;
    const ppe_size sz = 0;
    const ppe_size cpsz = 0;
    ppe_int ret = 0;

    if (! jnr || ! y || ! b || ! bsz || ! nbsz) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return -1;
    }

    if (jnr->n == 0) {
        ret = (*y)(ud, jnr->i, &s, &sz);
        if (ret < 0) {
            return ret;
        } else if (ret == 0) {
            goto PPE_SJN_JOIN_END;
        }
        if (*bsz < sz) {
            *nbsz = sz > *bsz ? sz : *bsz;
            ppe_err_set(PPE_ERR_TRY_AGAIN, NULL);
            return -1;
        }
        goto PPE_SJN_JOIN_NEXT;
    } /* if */

    do {
        if (*bsz - cpsz < jnr->dsz + sz) {
            *nbsz = jnr->dsz + sz > *bsz ? jnr->dsz + sz : *bsz;
            ppe_err_set(PPE_ERR_TRY_AGAIN, NULL);
            return -1;
        }
        memcpy(b + cpsz, jnr->d, jnr->dsz);
        cpsz += jnr->dsz;
        jnr->sz += jnr->dsz;

PPE_SJN_JOIN_NEXT:
        memcpy(b + cpsz, s, sz);
        cpsz += sz;
        jnr->sz += sz;
        jnr->i += 1;
        jnr->n += 1;
    } while ((ret = (*y)(ud, jnr->i, &s, &sz)) > 0);

PPE_SJN_JOIN_END:
    if (ret == 0) {
        jnr->i = 0;
    } /* if */
    *nbsz = *bsz;
    *bsz = cpsz;
    return ret;
} /* ppe_sjn_join */

typedef struct
{
    ppe_cstr_c s;
    ppe_size sz;
} ppe_sjn_cstr_data_st, *ppe_sjn_cstr_data;

static ppe_int ppe_sjn_yield_cstr(void * ud, ppe_uint idx, const ppe_char ** s, const ppe_size * sz)
{
    ppe_sjn_cstr_data d = (ppe_sjn_cstr_data) ud;
    *s = d->s;
    *sz = d->sz;
    return 0;
} /* ppe_sjn_yield_cstr */

PPE_API ppe_int ppe_sjn_append_cstr(ppe_sjn_joiner restrict jnr, ppe_sjn_action act, ppe_cstr_c restrict s, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_size * restrict nbsz)
{
    ppe_sjn_cstr_data_st ud;

    if (! s) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return -1;
    }

    ud.s = s;
    ud.sz = ppe_cs_size(s);
    if (act == PPE_SJN_MEASURE) {
        return ppe_sjn_measure(jnr, &ud, &ppe_sjn_yield_cstr, nbsz);
    }
    return ppe_sjn_join(jnr, &ud, &ppe_sjn_yield_cstr, b, bsz, nbsz);
} /* ppe_sjn_append_cstr */

PPE_API ppe_int ppe_sjn_append_cstr_with_size(ppe_sjn_joiner restrict jnr, ppe_sjn_action act, ppe_cstr_c restrict s, const ppe_size sz, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_size * restrict nbsz)
{
    ppe_sjn_cstr_data_st ud;

    if (! s) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return -1;
    }

    ud.s = s;
    ud.sz = sz;
    if (act == PPE_SJN_MEASURE) {
        return ppe_sjn_measure(jnr, &ud, &ppe_sjn_yield_cstr, nbsz);
    }
    return ppe_sjn_join(jnr, &ud, &ppe_sjn_yield_cstr, b, bsz, nbsz);
} /* ppe_sjn_append_cstr_with_size */

PPE_API ppe_int ppe_sjn_append_string(ppe_sjn_joiner restrict jnr, ppe_sjn_action act, ppe_string_c restrict s, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_size * restrict nbsz)
{
    ppe_sjn_cstr_data_st ud;

    if (! s) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return -1;
    }

    ud.s = ppe_str_addr(s);
    ud.sz = ppe_str_size(s);
    if (act == PPE_SJN_MEASURE) {
        return ppe_sjn_measure(jnr, &ud, &ppe_sjn_yield_cstr, nbsz);
    }
    return ppe_sjn_join(jnr, &ud, &ppe_sjn_yield_cstr, b, bsz, nbsz);
} /* ppe_sjn_append_string */

typedef struct
{
    ppe_cstr_c * strs;
    ppe_uint n;
} ppe_sjn_cstr_array_st, *ppe_sjn_cstr_array;

static ppe_int ppe_sjn_yield_cstr_from_array(void * ud, ppe_uint idx, const ppe_char ** s, const ppe_size * sz)
{
    ppe_sjn_cstr_array d = (ppe_sjn_cstr_array) ud;
    if (idx < d->n) {
        *s = d->strs[idx];
        *sz = ppe_cs_size(d->strs[idx]);
    } else {
        *s = NULL;
        *sz = 0;
    }
    return d->n - (idx + 1);
} /* ppe_sjn_yield_cstr_from_array */

PPE_API ppe_int ppe_sjn_append_cstrs(ppe_sjn_joiner restrict jnr, ppe_sjn_action act, ppe_cstr_c * restrict strs, const ppe_uint n, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_size * restrict nbsz)
{
    ppe_sjn_cstr_array_st ud;

    ud.c = strs;
    ud.n = n;
    if (act == PPE_SJN_MEASURE) {
        return ppe_sjn_measure(jnr, &ud, &ppe_sjn_yield_cstr_from_array, nbsz);
    }
    return ppe_sjn_join(jnr, &ud, &ppe_sjn_yield_cstr_from_array, b, bsz, nbsz);
} /* ppe_sjn_append_cstrs */

typedef struct
{
    ppe_string_c * strs;
    ppe_uint n;
} ppe_sjn_string_array_st, *ppe_sjn_string_array;

static ppe_int ppe_sjn_yield_cstr_from_string_array(void * ud, ppe_uint idx, const ppe_char ** s, const ppe_size * sz)
{
    ppe_sjn_string_array d = (ppe_sjn_string_array) ud;
    if (idx < d->n) {
        *s = ppe_str_addr(d->strs[idx]);
        *sz = ppe_cs_size(d->strs[idx]);
    } else {
        *s = NULL;
        *sz = 0;
    }
    return d->n - (idx + 1);
} /* ppe_sjn_yield_cstr_from_string_array */

PPE_API ppe_int ppe_sjn_append_strings(ppe_sjn_joiner restrict jnr, ppe_sjn_action act, ppe_string_c * restrict strs, const ppe_uint n, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_size * restrict nbsz)
{
    ppe_sjn_string_array_st ud;

    ud.c = strs;
    ud.n = n;
    if (act == PPE_SJN_MEASURE) {
        return ppe_sjn_measure(jnr, &ud, &ppe_sjn_yield_cstr_from_string_array, nbsz);
    }
    return ppe_sjn_join(jnr, &ud, &ppe_sjn_yield_cstr_from_string_array, b, bsz, nbsz);
} /* ppe_sjn_append_strings */

static ppe_int ppe_sjn_yield_cstr_from_snippet(void * ud, ppe_uint idx, const ppe_char ** s, const ppe_size * sz)
{
    ppe_cs_snippet spt = (ppe_cs_snippet) ud;
    if (idx < spt->cnt) {
        *s = spt->items[idx].s;
        *sz = spt->items[idx].sz;
    } else {
        *s = NULL;
        *sz = 0;
    } /* if */
    return spt->cnt - (idx + 1);
} /* ppe_sjn_yield_cstr_from_snippet */

PPE_API ppe_int ppe_sjn_append_snippet(ppe_sjn_joiner restrict jnr, ppe_sjn_action act, ppe_cs_snippet restrict spt, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_size * restrict nbsz)
{
    if (act == PPE_SJN_MEASURE) {
        return ppe_sjn_measure(jnr, spt, &ppe_sjn_yield_cstr_from_snippet, nbsz);
    }
    return ppe_sjn_join(jnr, spt, &ppe_sjn_yield_cstr_from_snippet, b, bsz, nbsz);
} /* ppe_sjn_append_snippet */

#ifdef __cplusplus
}
#endif

