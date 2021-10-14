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

/* ==== Definitions : C-String ============================================== */

/* ---- Preset Values ------------------------------------------------------- */

static ppe_cstr_c const cs_empty_s = "";

/* ---- Functions ----------------------------------------------------------- */

/* -- Internals -- */

#define PPE_CONF_STR_SNIPPET_MAX 64

//static ppe_cs_snippet cs_split(ppe_cstr_c const restrict s, ppe_cstr_c const restrict d, const ppe_size dsz, ppe_cs_snippet restrict spt, ppe_uint * restrict n, ppe_cstr_c * restrict ss, const ppe_str_option opt)
//{
//    ppe_cs_snippet nw = NULL;
//    ppe_cstr_c p = 0;
//    ppe_cstr_c q = 0;
//    ppe_size sz = 0;
//    ppe_uint i = 0;
//    ppe_uint max = PPE_CONF_STR_SNIPPET_MAX; /* Dont search infinitely. */
//
//    if (n) {
//        if (n == 0) {
//            /* Cannot use a null or empty string as delimiter. */
//            ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//            return NULL;
//        }
//        max = *n;
//    }
//
//    p = (ss && *ss) ? *ss : s;
//    if (! spt) {
//        /* NEW-SNIPPET MODE */
//        if (ppe_cs_is_empty(p)) {
//            return &cspt_empty_s;
//        }
//
//        /* Count the number of substrings. */
//        for (i = 0; i < max - 1 && (q = ppe_cs_find(p, d)); i += 1) {
//            p = q + dsz;
//        } /* for */
//
//        /* Counted N delimiters, so there must be N+1 substrings, including empty ones. */
//        i += 1;
//
//        nw = ppe_cspt_create(i);
//        if (! nw) {
//            ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
//            return NULL;
//        }
//
//        p = (ss && *ss) ? *ss : s;
//    } else {
//        /* FILL-SNIPPET MODE */
//        ppe_cspt_reset(spt);
//        if (ppe_cs_is_empty(p)) {
//            ppe_cspt_append(spt, cs_empty_s, 0);
//            return spt;
//        }
//
//        i = ppe_cspt_capacity(spt);
//        nw = spt;
//    } /* if */
//
//    if (i < max) {
//        max = i;
//    }
//    for (i = 0; i < max - 1 && (q = ppe_cs_find(p, d)); i += 1) {
//        sz = q - p;
//        ppe_cspt_append(nw, p, sz);
//        p = q + dsz;
//    } /* for */
//
//    if (i == max - 1 && (q = ppe_cs_find(p, d)) && ss) {
//        sz = q - p;
//        ppe_cspt_append(nw, p, sz);
//        p += sz;
//        *ss = p;
//    } else {
//        sz = ppe_cs_size(p);
//        ppe_cspt_append(nw, p, sz);
//        p += sz;
//        if (ss) {
//            *ss = NULL;
//        }
//    } /* if */
//    if (n) {
//        *n = i + 1;
//    }
//    return nw;
//} /* cs_split */

//static ppe_cstr_c cs_replace(ppe_cstr_c const restrict s, const ppe_size sz, const ppe_size off, ppe_size ssz, ppe_cstr_c const restrict to, const ppe_size tosz, ppe_cstr restrict b, ppe_size * restrict bsz, const ppe_str_option opt)
//{
//    ppe_size cpsz = 0;
//
//    if (sz < off) {
//        ppe_err_set(PPE_ERR_OUT_OF_RANGE, NULL);
//        return NULL;
//    }
//
//    if (sz < off + ssz) {
//        /* Shrink the range of the substring. */
//        ssz -= (off + ssz) - sz;
//    }
//    cpsz = sz - ssz + tosz;
//
//    if (! b) {
//        if (bsz) {
//            /* MEASURE MODE */
//            *bsz = cpsz + 1; /* Include the terminating NUL byte. */
//            return s;
//        }
//
//        /* NEW-STRING MODE */
//        b = ppe_mp_malloc(cpsz + 1); /* Include the terminating NUL byte. */
//        if (! b) {
//            ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
//            return NULL;
//        }
//    } else {
//        /* FILL-BUFFER MODE */
//        if (! bsz) {
//            ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//            return NULL;
//        }
//        if (*bsz < cpsz + 1) {
//            ppe_err_set(PPE_ERR_OUT_OF_CAPACITY, NULL);
//            return NULL;
//        }
//        *bsz = cpsz;
//    } /* if */
//
//    /* The part before the substring which is being replaced. */
//    memcpy(b, s, off);
//
//    /* The part which is replacing the substring. */
//    memcpy(b + off, to, tosz);
//
//    /* The part after the substring which is being replaced. */
//    memcpy(b + off + tosz, s + (off + ssz), sz - (off + ssz));
//
//    b[cpsz] = '\0';
//    return b;
//} /* cs_replace */

#define PPE_CONF_STR_SUBSTITUTE_MAX 256

//static ppe_cstr_c cs_substitute(ppe_cstr_c const restrict s, ppe_cstr_c const restrict from, const ppe_size frsz, ppe_cstr_c const to, const ppe_size tosz, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_uint * restrict n, const ppe_str_option opt)
//{
//    ppe_cstr_c p = NULL;
//    ppe_cstr_c q = NULL;
//    ppe_size rmsz = 0;
//    ppe_size cpsz = 0;
//    ppe_uint i = 0;
//    ppe_uint max = PPE_CONF_STR_SUBSTITUTE_MAX;
//
//    if (n) {
//        if (*n == 0) {
//            ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//            return NULL;
//        }
//        if (*n < max) {
//            max = *n;
//        }
//    } /* if */
//
//    if (! b) {
//        p = s;
//        if (frsz != tosz) {
//            for (i = 0; i < max && (q = ppe_cs_find(p, from)); i += 1, p = q + frsz) {
//                cpsz += (q - p) + tosz;
//            } /* while */
//        } /* if */
//
//        rmsz = ppe_cs_size(p);
//        cpsz += rmsz;
//
//        if (bsz) {
//            /* MEASURE-SIZE MODE */
//            *bsz = cpsz + 1; /* Include the terminating NUL byte. */
//            if (n) { *n = i; }
//            ppe_err_set(PPE_ERR_TRY_AGAIN, NULL);
//            return NULL;
//        } /* if */
//
//        /* NEW-STRING MODE */
//        if (i == 0) {
//            /* Subtitute no substrings. */
//            if (n) { *n = i; }
//            return ppe_cs_create(s, cpsz);
//        } /* if */
//
//        b = (ppe_cstr) ppe_mp_malloc(cpsz + 1); /* Include the terminating NUL byte. */
//        if (! b) {
//            ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
//            return NULL;
//        }
//        cpsz = 0;
//        max = i;
//    } else {
//        /* FILL-BUFFER */
//        if (b == s || b == from || b == to) {
//            ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//            return NULL;
//        }
//        if (! bsz) {
//            ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//            return NULL;
//        }
//    } /* if */
//
//    p = s;
//    for (i = 0; i < max && (q = ppe_cs_find(p, from)); i += 1, p = q + frsz) {
//        if (*bsz < cpsz + (q - s)) {
//            ppe_err_set(PPE_ERR_OUT_OF_CAPACITY, NULL);
//            return NULL;
//        }
//        memcpy(b + cpsz, p, q - p);
//        cpsz += q - p;
//
//        if (*bsz < cpsz + tosz) {
//            ppe_err_set(PPE_ERR_OUT_OF_CAPACITY, NULL);
//            return NULL;
//        }
//        memcpy(b + cpsz, to, tosz);
//        cpsz += tosz;
//    } /* for */
//
//    rmsz = ppe_cs_size(p);
//    if (*bsz < cpsz + rmsz + 1) {
//        ppe_err_set(PPE_ERR_OUT_OF_CAPACITY, NULL);
//        return NULL;
//    }
//    memcpy(b + cpsz, p, rmsz);
//    cpsz += rmsz;
//
//    b[cpsz] = '\0';
//    if (bsz) { *bsz = cpsz; }
//    if (n) { *n = i; }
//    return b;
//} /* cs_substitute */

/* -- Preset values -- */

PPE_API ppe_cstr_c ppe_cs_empty(void)
{
    return cs_empty_s;
} /* ppe_cs_empty */

/* -- Property -- */

PPE_API ppe_size ppe_cs_size(ppe_cstr_c restrict s)
{
    return strlen(s);
} /* ppe_cs_size */

/* -- Create & Destroy -- */

PPE_API ppe_cstr_c ppe_cs_create(ppe_cstr_c const restrict s, const ppe_size sz)
{
    ppe_cstr nw = NULL;

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
} /* ppe_cs_create */

PPE_API void ppe_cs_destroy(ppe_cstr_c restrict s)
{
    if (s && s != cs_empty_s) {
        ppe_mp_free((void *)s);
    }
} /* ppe_cs_destroy */

/* -- Test -- */

PPE_API ppe_bool ppe_cs_is_empty(ppe_cstr_c restrict s)
{
    return (s[0] == '\0');
} /* ppe_cs_is_empty */

PPE_API ppe_int ppe_cs_compare(ppe_cstr_c restrict s1, ppe_cstr_c restrict s2)
{
    return strcmp(s1, s2);
} /* ppe_cs_compare */

/* -- Substring -- */

PPE_API ppe_cstr_c ppe_cs_find(ppe_cstr_c restrict s, ppe_cstr_c restrict t)
{
    return (ppe_cstr_c) strstr(s, t);
} /* ppe_cs_find */

static ppe_cstr_c cs_dupstr(ppe_cstr_c const restrict s, ppe_size cpsz, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt)
{
    if (! b) {
        if (bsz) {
            /* MEASURE-SIZE MODE */
            *bsz = cpsz; /* Exclude the terminating NUL byte. */
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
    if (*bsz < cpsz) {
        if (opt & PPE_STR_OPT_DONT_TRUNCATE) {
            /* Caller wants a full copy but the buffer would be out of space. */
            *bsz = cpsz;
            ppe_err_set(PPE_ERR_OUT_OF_BUFFER, NULL);
            return NULL;
        }
        cpsz = *bsz;
    } /* if */
    memcpy(b, s, cpsz);
    *bsz = cpsz;
    return b;
} /* cs_dupstr */

PPE_API ppe_cstr_c ppe_cs_slice(ppe_cstr_c const restrict s, const ppe_size off, const ppe_size rsz, ppe_cstr restrict b, ppe_size * bsz, ppe_str_option opt)
{
    ppe_size sz = 0;
    ppe_size cpsz = 0;

    sz = ppe_cs_size(s);
    if (sz < off) {
        ppe_err_set(PPE_ERR_OUT_OF_RANGE, NULL);
        return NULL;
    }

    cpsz = sz < off + rsz ? sz - off : rsz;
    return cs_dupstr(s + off, cpsz, b, bsz, opt);
} /* ppe_cs_slice */

/* -- Trim & Chomp -- */

static ppe_cstr_c cs_get_trim_position(ppe_cstr_c restrict s, const ppe_size sz, ppe_cstr_c restrict t, ppe_size * restrict cpsz, const ppe_str_option opt)
{
    ppe_cstr_c p = NULL;
    ppe_cstr_c q = NULL;

    p = s;
    q = s + sz - 1;
    if (opt & PPE_STR_OPT_LEFT_END) {
        while (*p && strchr(t, *p)) { ++p; }
    } /* if */
    if (opt & PPE_STR_OPT_RIGHT_END) {
        while (p <= q && strchr(t, *q)) { --q; }
    } /* if */
    if (q < p) {
        *cpsz = 0;
        return cs_empty_s;
    }
    *cpsz = q - p + 1;
    return p;
} /* cs_get_trim_position */

PPE_API ppe_cstr_c ppe_cs_trim(ppe_cstr_c const restrict s, ppe_cstr_c restrict t, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt)
{
    ppe_cstr_c p = NULL;
    ppe_size cpsz = 0;

    if (s == t) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, "Target string of bytes/characters to be trimmed cannot be the same of the source one.");
        return NULL;
    }
    if (ppe_cs_is_empty(t)) {
        t = PPE_STR_SPACES;
    }

    p = cs_get_trim_position(s, ppe_cs_size(s), t, &cpsz, opt);
    return cs_dupstr(p, cpsz, b, bsz, opt);
} /* ppe_cs_trim */

PPE_API ppe_cstr_c ppe_cs_chop(ppe_cstr_c const restrict s, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt)
{
    ppe_size cpsz = 0;

    if (ppe_cs_is_empty(s)) {
        cpsz = 0;
    } else {
        /* TODO: What if chopping a UTF-8 character? */
        cpsz = ppe_cs_size(s) - 1;
    } /* if */
    return cs_dupstr(s, cpsz, b, bsz, opt);
} /* ppe_cs_chop */

static ppe_cstr_c cs_get_chomp_position(ppe_cstr_c const restrict s, const ppe_size sz, ppe_cstr_c const restrict t, const ppe_size tsz, const ppe_int n, ppe_size * restrict cpsz)
{
    ppe_size rsz = sz;
    ppe_int i = n;

    do {
        if (strncmp(s + rsz - tsz, t, tsz) != 0) {
            break;
        }
        rsz -= tsz;
    } while ((n <= 0 || --i > 0) && rsz >= tsz);
    *cpsz = rsz;
    return s;
} /* cs_get_chomp_position */

PPE_API ppe_cstr_c ppe_cs_chomp(ppe_cstr_c const restrict s, ppe_cstr_c const restrict t, const ppe_int n, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt)
{
    ppe_cstr_c p = NULL;
    ppe_size sz = 0;
    ppe_size cpsz = 0;

    sz = ppe_cs_size(s);
    if (ppe_cs_is_empty(t)) {
        return cs_dupstr(s, sz, b, bsz, opt);
    }
    if (ppe_cs_is_empty(s)) {
        return cs_dupstr(cs_empty_s, 0, b, bsz, opt);
    }
    p = cs_get_chomp_position(s, sz, t, ppe_cs_size(t), n, &cpsz);
    return cs_dupstr(p, cpsz, b, bsz, opt);
} /* ppe_cs_chomp */

///* -- Join & Concat -- */
//
//static ppe_cstr_c cs_join_s(ppe_cstr_c restrict d, ppe_size dsz, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt, va_list * restrict ap)
//{
//    va_list cp;
//    ppe_bool ret = ppe_false;
//    ppe_cstr s = NULL;
//    ppe_size sz = 0;
//    ppe_cstr nw = NULL;
//    ppe_sjn_joiner jnr = NULL;
//    ppe_str_action act = PPE_STR_JOIN_END;
//
//    jnr = ppe_sjn_create(d, dsz);
//    if (! jnr) {
//        return NULL;
//    }
//
//    /* -- Check all arugments and measure the total bytes needed to hold the result. -- */
//    va_copy(cp, *ap);
//    while ((act = va_arg(cp, ppe_str_action)) != PPE_STR_JOIN_END) {
//        switch (act) {
//            case PPE_STR_JOIN_CSTR:
//                s = va_arg(cp, ppe_cstr);
//                if (! s) {
//                    ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//                    goto CS_JOIN_S_ERROR;
//                }
//                if (ppe_sjn_append_cstr(jnr, PPE_SJN_MEASURE, s, ppe_cs_size(s), NULL, NULL, NULL) < 0) {
//                    goto CS_JOIN_S_ERROR;
//                } /* if */
//                break;
//
//            case PPE_STR_JOIN_CSTR_WITH_SIZE:
//                break;
//
//            case PPE_STR_JOIN_CSTR_ARRAY:
//                break;
//
//            case PPE_STR_JOIN_STRING:
//                break;
//
//            case PPE_STR_JOIN_STRING_ARRAY:
//                break;
//
//            case PPE_STR_JOIN_SNIPPET:
//                break;
//
//            default:
//                ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//                return NULL;
//        } /* switch */
//    } /* while */
//    va_end(cp);
//
//    if (! ret) {
//        return NULL;
//    }
//
//    if (b) {
//        if (! bsz) {
//            ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//            return NULL;
//        }
//        if (*bsz < args.cpsz + 1) {
//            ppe_err_set(PPE_ERR_OUT_OF_CAPACITY, NULL);
//            return NULL;
//        }
//
//        if ((args.cpsz - dsz) == 0) {
//            b[0] = '\0';
//            return b;
//        }
//
//        args.b = b;
//    } else {
//        if (bsz) {
//            /* MEASURE-SIZE MODE */
//            *bsz = args.cpsz + 1; /* Include the terminating NUL character. */
//            ppe_err_set(PPE_ERR_TRY_AGAIN, NULL);
//            return NULL;
//        }
//
//        /* NEW-STRING MODE */
//        if ((args.cpsz - dsz) == 0) {
//            return cs_empty_s;
//        }
//
//        nw = (ppe_cstr) ppe_mp_malloc(args.cpsz + 1); /* Include the terminating NUL character. */
//        if (! nw) {
//            ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
//            return NULL;
//        }
//
//        args.b = nw;
//    } /* if */
//
//    args.cpsz = 0;
//
//    va_copy(cp, *ap);
//    args.cp = &cp;
//    cs_join_act(copy_s, &args);
//    va_end(cp);
//
//    args.cpsz -= dsz;
//
//    if (! bsz) {
//        /* NEW-STRING MODE */
//        nw[args.cpsz] = '\0';
//    } else {
//        /* FILL-BUFFER MODE */
//        if (args.cpsz + 1 <= *bsz) {
//            nw[args.cpsz] = '\0';
//        }
//        *bsz = args.cpsz;
//    } /* if */
//    return (ppe_cstr_c) nw;
//
//CS_JOIN_S_ERROR;
//    ppe_sjn_destroy(jnr);
//    return NULL;
//} /* cs_join_s */
// 
//PPE_API ppe_cstr_c ppe_cs_join(ppe_cstr_c const restrict d, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt, ...)
//{
//    ppe_cstr_c ret = NULL;
//    va_list args;
//
//    va_start(args, opt);
//    ret = cs_join(d, ppe_cs_size(d), b, bsz, opt, &args);
//    va_end(args);
//    return ret;
//} /* ppe_cs_join */
// 
//PPE_API ppe_cstr_c ppe_cs_concat(ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt, ...)
//{
//    ppe_cstr_c ret = NULL;
//    va_list args;
//
//    va_start(args, opt);
//    ret = cs_join(cs_empty_s, 0, b, bsz, opt, &args);
//    va_end(args);
//    return ret;
//} /* ppe_cs_concat */
//
///* -- Split & Slice -- */
//
//PPE_API ppe_cs_snippet ppe_cs_split(const ppe_cstr restrict s, const ppe_cstr restrict d, ppe_cs_snippet restrict spt, ppe_uint * restrict n, ppe_cstr_c * restrict ss, const ppe_str_option opt)
//{
//    if (! s || ! d || s == d) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return NULL;
//    }
//    if (ppe_cs_is_empty(d)) {
//        /* Cannot use a null or empty string as delimiter. */
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return NULL;
//    }
//    return cs_split(s, d, ppe_cs_size(d), spt, n, ss, opt);
//} /* ppe_cs_split */
//
///* -- Replace & Substitute -- */
//
//PPE_API ppe_cstr_c ppe_cs_replace(ppe_cstr_c const restrict s, const ppe_size off, const ppe_size ssz, ppe_cstr_c const restrict to, ppe_cstr restrict b, ppe_size * restrict bsz, const ppe_str_option opt)
//{
//    if (! s || ! to || to == s || b == s) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return NULL;
//    }
//    return cs_replace(s, ppe_cs_size(s), off, ssz, to, ppe_cs_size(to), b, bsz, opt);
//} /* ppe_cs_replace */
//
//PPE_API ppe_cstr_c ppe_cs_substitute(ppe_cstr_c const restrict s, ppe_cstr_c const restrict from, ppe_cstr_c const restrict to, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_uint * restrict n, const ppe_str_option opt)
//{
//    if (! s || ! from || ! to || ppe_cs_is_empty(from)) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return NULL;
//    }
//    if (s == from || s == to || from == to) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return NULL;
//    }
//    return cs_substitute(s, from, ppe_cs_size(from), to, ppe_cs_size(to), b, bsz, n, opt);
//} /* ppe_cs_substitute */
//
//static ppe_cstr cs_sprintf(ppe_cstr restrict b, ppe_size * restrict bsz, const ppe_cstr restrict fmt, va_list * restrict ap)
//{
//    int ret = 0;
//
//    ret = vsnprintf(b, *bsz, fmt, *ap);
//    if (ret < 0) {
//        /* TODO: Error handling. */
//        return NULL;
//    }
//    *bsz = ((ppe_size) ret); /* Excluding the terminating '\0' byte. */
//    return b;
//} /* cs_sprintf */
//
//PPE_API ppe_cstr ppe_cs_sprintf(ppe_cstr restrict b, ppe_size * restrict bsz, const ppe_str_option opt, const ppe_cstr restrict fmt, ...)
//{
//    ppe_cstr ret = NULL;
//    ppe_cstr nw = NULL;
//    ppe_size nwsz = 128;
//    ppe_size fmsz = 0;
//    ppe_size obsz = *bsz;
//    va_list args;
//
//    if (b) {
//        fmsz = obsz;
//
//        va_start(args, fmt);
//        ret = cs_sprintf(b, &fmsz, fmt, &args);
//        va_end(args);
//
//        if (! ret) {
//            return NULL;
//        }
//
//        if (obsz <= fmsz && (opt & PPE_STR_OPT_DONT_TRUNCATE)) {
//            /* The output has been truncated but the caller want a fully formatted string. */
//            ppe_err_set(PPE_ERR_OUT_OF_CAPACITY, NULL);
//            return NULL;
//        }
//
//        *bsz = fmsz;
//        return b;
//    }
//
//#if defined(PPE_CONF_SUPPORT_SUSV2)
//    nw = (ppe_cstr) ppe_mp_malloc(nwsz);
//    if (! nw) {
//        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
//        return NULL;
//    }
//#endif
//
//    /* Detect the number of characters which would have been written to the final string if enough space have been available. */
//    fmsz = nwsz;
//
//    va_start(args, fmt);
//    ret = cs_sprintf(nw, &fmsz, fmt, &args);
//    va_end(args);
//
//    if (! ret) {
//#if defined(PPE_CONF_SUPPORT_SUSV2)
//        ppe_mp_free(nw);
//#endif
//        return NULL;
//    }
//
//    if (opt & PPE_STR_OPT_NEW_STRING) {
//        /* The caller want a new string. */
//
//#if defined(PPE_CONF_SUPPORT_SUSV2)
//        if (fmsz < nwsz) {
//            ret = (ppe_cstr) ppe_mp_malloc(nw, fmsz + 1); /* Allocate a new buffer to avoid wasting memory. */
//            if (ret) {
//                /* Copy and replace the final string. */
//                memcpy(ret, nw, fmsz + 1);
//                ppe_mp_free(nw);
//                nw = ret;
//            }
//            *bsz = fmsz; /* Excluding the terminating '\0' byte. */
//            ppe_err_set(PPE_ERR_SUCCEED, NULL);
//            return nw;
//        }
//
//        fmsz += 1; /* Including the terminating '\0' byte. */
//
//        /* The final string would have been truncated. */
//        ret = (ppe_cstr) ppe_mp_realloc(nw, fmsz); /* Expand the buffer for reformatting. */
//        if (! ret) {
//            ppe_mp_free(nw);
//            ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
//            return NULL;
//        }
//        nw = ret;
//#else
//        nw = (ppe_cstr) ppe_mp_malloc(fmsz);
//        if (! nw) {
//            ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
//            return NULL;
//        }
//#endif
//
//        /* Try to reformat again. */
//        va_start(args, fmt);
//        ret = cs_sprintf(nw, &fmsz, fmt, &args); /* The terminating '\0' byte shall be written. */
//        va_end(args);
//
//        if (! ret) {
//            ppe_mp_free(nw);
//            return NULL;
//        }
//
//        b = nw;
//    } /* if */
//
//    *bsz = fmsz; /* Excluding the terminating '\0' byte. */
//    ppe_err_set(PPE_ERR_SUCCEED, NULL);
//    return b; /* Return Whatever the caller passed. */
//}
//
///* ==== Definitions : String ================================================ */
//
///* ---- Types --------------------------------------------------------------- */
//
//typedef struct PPE_STRING
//{
//    ppe_size sz;
//    ppe_char buf[1];
//} ppe_string_st;
//
///* ---- Preset Values ------------------------------------------------------- */
//
//static ppe_string_st str_empty_s = { 0, {'\0'}};
//
///* ---- Functions ----------------------------------------------------------- */
//
///* -- Internals -- */
//
//static inline void ppe_str_get_var_args_with_size(const ppe_str_option opt, const void * restrict v, const ppe_size vsz, ppe_cstr_c * restrict t, ppe_size * tsz)
//{
//    if (opt & PPE_STR_OPT_CSTR_ARG) {
//        *t = (ppe_cstr_c) v;
//        *tsz = vsz;
//    } else {
//        *t = ((const ppe_string) v)->buf;
//        *tsz = ((const ppe_string) v)->sz;
//    } /* if */
//} /* ppe_str_get_var_args_with_size */
//
///* -- Preset values -- */
//
//PPE_API const ppe_string ppe_str_get_empty(void)
//{
//    return &str_empty_s;
//}
//
///* -- Property -- */
//
//static inline ppe_cstr_c str_addr(ppe_string_c restrict s)
//{
//    return s->buf;
//} /* str_addr */
//
//PPE_API ppe_cstr_c ppe_str_addr(ppe_string_c restrict s)
//{
//    assert(s);
//    return str_addr(s);
//} /* ppe_str_addr */
//
//static inline ppe_size str_size(ppe_string_c restrict s)
//{
//    return s->sz;
//} /* str_size */
//
//PPE_API ppe_size ppe_str_size(ppe_string_c restrict s)
//{
//    assert(s);
//    return str_size(s);
//} /* ppe_str_size */
//
///* -- Test -- */
//
//static inline ppe_bool str_is_empty(ppe_string_c restrict s)
//{
//    return (s->sz == 0);
//} /* str_is_empty */
//
//PPE_API ppe_bool ppe_str_is_empty(ppe_string_c restrict s)
//{
//    assert(s);
//    return str_is_empty(s);
//} /* ppe_str_is_empty */
//
//PPE_API ppe_bool ppe_str_equals_to(ppe_string_c restrict s1, ppe_string_c restrict s2)
//{
//    assert(s1);
//    assert(s2);
//    return strcmp(s1->buf, s2->buf) == 0;
//} /* ppe_str_equals_to */
//
//PPE_API ppe_bool ppe_str_less_than(ppe_string_c restrict s1, ppe_string_c restrict s2)
//{
//    assert(s1);
//    assert(s2);
//    return strcmp(s1->buf, s2->buf) < 0;
//} /* ppe_str_less_than */
//
//PPE_API ppe_bool ppe_str_greater_than(ppe_string_c restrict s1, ppe_string_c restrict s2)
//{
//    assert(s1);
//    assert(s2);
//    return strcmp(s1->buf, s2->buf) > 0;
//} /* ppe_str_greater_than */
//
///* -- Create & Destroy -- */
//
//static ppe_string str_create(ppe_cstr_c restrict s, ppe_size sz)
//{
//    ppe_string nw = NULL;
//
//    nw = (ppe_string) ppe_mp_malloc(sizeof(ppe_string_st) + sz); /* Including the terminating NUL byte. */
//    if (! nw) {
//        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
//        return NULL;
//    }
//
//    memcpy(nw->buf, s, sz);
//    nw->buf[sz] = '\0';
//    nw->sz = sz;
//    return nw;
//} /* str_create */
//
//PPE_API ppe_string ppe_str_create(ppe_cstr_c restrict s, ppe_ssize sz)
//{
//    ppe_size nwsz = 0;
//
//    if (! s) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return NULL;
//    }
//    
//    nwsz = sz < 0 ? ppe_cs_size(s) : (ppe_size)sz;
//    if (nwsz == 0) {
//        return &str_empty_s;
//    }
//    return str_create(s, nwsz);
//} /* ppe_str_create */
//
//PPE_API ppe_string ppe_str_clone(ppe_string_c restrict s)
//{
//    if (! s) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return NULL;
//    }
//    if (str_is_empty(s)) {
//        return &str_empty_s;
//    }
//    return ppe_str_create(s->buf, s->sz);
//} /* ppe_str_clone */
//
//PPE_API void ppe_str_destroy(ppe_string restrict s)
//{
//    if (s && s != &str_empty_s) {
//        ppe_mp_free(s);
//    }
//} /* ppe_str_destroy */
//
///* -- Manipulate -- */
//
//PPE_API ppe_string ppe_str_substr(ppe_string_c restrict s, const ppe_size start, const ppe_size rsz, const ppe_str_option opt)
//{
//    if (! s) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return NULL;
//    }
//    if (s->sz <= start) {
//        return &str_empty_s;
//    }
//    return str_create(s->buf + start, (s->sz - start < rsz) ? (s->sz - start) : rsz);
//} /* ppe_str_substr */
//
///* ---- */
//
//PPE_API ppe_string ppe_str_trim_chars_cs(ppe_string_c restrict s, ppe_cstr_c restrict t, const ppe_str_option opt)
//{
//    ppe_cstr_c p = NULL;
//    ppe_size cpsz = 0;
//
//    if (! s || ! (opt & (PPE_STR_OPT_LEFT_END | PPE_STR_OPT_RIGHT_END)) || str_addr(s) == t) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return NULL;
//    }
//    if (str_is_empty(s)) {
//        return &str_empty_s;
//    }
//
//    p = cs_get_trim_position(str_addr(s), str_size(s), t ? t : PPE_STR_SPACES, &cpsz, opt);
//    if (ppe_cs_is_empty(p)) {
//        return &str_empty_s;
//    }
//    return str_create(p, cpsz);
//} /* ppe_str_trim_chars_cs */
//
///* ---- */
//
//PPE_API ppe_string ppe_str_chop(ppe_string_c restrict s, const ppe_str_option opt)
//{
//    if (! s) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return NULL;
//    }
//    if (str_is_empty(s)) {
//        return &str_empty_s;
//    }
//    return ppe_str_create(s->buf, s->sz - 1);
//} /* ppe_str_chop */

///* -- Join & Concat -- */
//
//static ppe_string ppe_str_join_imp(const ppe_string restrict d, const ppe_str_option opt, va_list * restrict args)
//{
//    ppe_cstr_c ret = NULL;
//    ppe_string nw = NULL;
//    ppe_size nwsz = 0;
//    va_list cp;
//
//    if (! d) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return NULL;
//    }
//
//    va_copy(cp, *args);
//    ret = cs_join(d->buf, d->sz, NULL, &nwsz, opt, &cp);
//    va_end(cp);
//    if (! ret) {
//        return NULL;
//    }
//
//    nw = (ppe_string) ppe_mp_malloc(sizeof(ppe_string_st) + nwsz - 1); /* The terminating NUL charater has been counted twice. */
//    if (! nw) {
//        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
//        return NULL;
//    }
//
//    va_copy(cp, *args);
//    ret = cs_join(d->buf, d->sz, nw->buf, &nwsz, opt, &cp); /* Include the terminating NUL character. */
//    va_end(cp);
//    if (! ret) {
//        ppe_mp_free(nw);
//        return NULL;
//    }
//
//    nw->sz = nwsz;
//    return nw;
//} /* ppe_str_join_imp */
//
//PPE_API ppe_string ppe_str_join(const ppe_string restrict d, const ppe_str_option opt, ...)
//{
//    ppe_string ret = NULL;
//    va_list args;
//
//    if (! d) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return NULL;
//    }
//
//    va_start(args, opt);
//    ret = ppe_str_join_imp(d, opt, &args);
//    va_end(args);
//    return ret;
//} /* ppe_str_join */
//
//PPE_API ppe_string ppe_str_concat(const ppe_str_option opt, ...)
//{
//    ppe_string ret = NULL;
//    va_list args;
//
//    va_start(args, opt);
//    ret = ppe_str_join_imp(&str_empty_s, opt, &args);
//    va_end(args);
//    return ret;
//} /* ppe_str_concat */
//
///* -- Split -- */
//
//PPE_API ppe_cs_snippet ppe_str_split(const ppe_string restrict s, const ppe_string restrict d, ppe_cs_snippet restrict spt, ppe_uint * restrict n, const ppe_str_option opt)
//{
//    if (! s || ! d || s == d) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return NULL;
//    }
//    if (ppe_str_is_empty(d)) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return NULL;
//    }
//    return cs_split(s->buf, d->buf, d->sz, spt, n, NULL, opt);
//}
//
//PPE_API ppe_cs_snippet ppe_str_split_cs(const ppe_string restrict s, const ppe_cstr restrict d, ppe_cs_snippet restrict spt, ppe_uint * restrict n, const ppe_str_option opt)
//{
//    if (! s || ! d) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return NULL;
//    }
//    if (ppe_cs_is_empty(d)) {
//        /* Cannot use a null or empty string as delimiter. */
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return NULL;
//    }
//    return cs_split(s->buf, d, ppe_cs_size(d), spt, n, NULL, opt);
//}
//
///* -- Replace & Substitute -- */
//
//PPE_API ppe_string ppe_str_replace(const ppe_string restrict s, const ppe_size off, const ppe_size ssz, const ppe_string restrict to, const ppe_str_option opt)
//{
//    ppe_cstr_c ret = NULL;
//    ppe_string nw = NULL;
//    ppe_size nwsz = 0;
//
//    if (! s || ! to || to == s) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return NULL;
//    }
//
//    ret = cs_replace(s->buf, s->sz, off, ssz, to->buf, to->sz, NULL, &nwsz, opt);
//    if (! ret && ppe_err_get_code() != PPE_ERR_TRY_AGAIN) {
//        return NULL;
//    }
//
//    if (nwsz == 1) {
//        return &str_empty_s;
//    }
//
//    nw = (ppe_string) ppe_mp_malloc(sizeof(ppe_string_st) + nwsz - 1); /* The terminating NUL byte has been counted twice. */
//    if (! nw) {
//        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
//        return NULL;
//    }
//
//    ret = cs_replace(s->buf, s->sz, off, ssz, to->buf, to->sz, nw->buf, &nwsz, opt);
//    if (! ret) {
//        ppe_mp_free(nw);
//        return NULL;
//    }
//    nw->sz = nwsz;
//    return nw;
//} /* ppe_str_replace */
//
//PPE_API ppe_string ppe_str_replace_cs(const ppe_string restrict s, const ppe_size off, const ppe_size ssz, const ppe_cstr restrict to, const ppe_str_option opt)
//{
//    ppe_cstr_c ret = NULL;
//    ppe_string nw = NULL;
//    ppe_size nwsz = 0;
//    ppe_size tosz = 0;
//
//    if (! s || ! to) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return NULL;
//    }
//
//    tosz = ppe_cs_size(to);
//
//    ret = cs_replace(s->buf, s->sz, off, ssz, to, tosz, NULL, &nwsz, opt);
//    if (! ret && ppe_err_get_code() != PPE_ERR_TRY_AGAIN) {
//        return NULL;
//    }
//
//    if (nwsz == 1) {
//        return &str_empty_s;
//    }
//
//    nw = (ppe_string) ppe_mp_malloc(sizeof(ppe_string_st) + nwsz - 1); /* The terminating NUL byte has been counted twice. */
//    if (! nw) {
//        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
//        return NULL;
//    }
//
//    ret = cs_replace(s->buf, s->sz, off, ssz, to, tosz, nw->buf, &nwsz, opt);
//    if (! ret) {
//        ppe_mp_free(nw);
//        return NULL;
//    }
//    nw->sz = nwsz;
//    return nw;
//} /* ppe_str_replace_cs */
//
//PPE_API ppe_string ppe_str_substitute(const ppe_string restrict s, const ppe_string restrict from, const ppe_string restrict to, ppe_uint * restrict n, const ppe_str_option opt)
//{
//    ppe_cstr_c ret = NULL;
//    ppe_string nw = NULL;
//    ppe_size nwsz = 0;
//
//    if (! s || ! from || ! to || s == from || s == to || from == to) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return NULL;
//    }
//    if (ppe_str_is_empty(from)) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return NULL;
//    }
//
//    ret = cs_substitute(s->buf, from->buf, from->sz, to->buf, to->sz, NULL, &nwsz, n, opt);
//    if (! ret && ppe_err_get_code() != PPE_ERR_TRY_AGAIN) {
//        return NULL;
//    }
//
//    if (nwsz == 1) {
//        return &str_empty_s;
//    }
//
//    nw = (ppe_string) ppe_mp_malloc(sizeof(ppe_string_st) + nwsz - 1); /* The terminating NUL byte has been counted twice. */
//    if (! nw) {
//        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
//        return NULL;
//    }
//
//    ret = cs_substitute(s->buf, from->buf, from->sz, to->buf, to->sz, nw->buf, &nwsz, n, opt);
//    if (! ret) {
//        ppe_mp_free(nw);
//        return NULL;
//    }
//    nw->sz = nwsz;
//    return nw;
//} /* ppe_str_substitute */
//
//PPE_API ppe_string ppe_str_substitute_cs(const ppe_string restrict s, const ppe_cstr restrict from, const ppe_cstr restrict to, ppe_uint * restrict n, const ppe_str_option opt)
//{
//    ppe_cstr_c ret = NULL;
//    ppe_string nw = NULL;
//    ppe_size nwsz = 0;
//    ppe_size frsz = 0;
//    ppe_size tosz = 0;
//
//    if (! s || ! from || ! to || from == to) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return NULL;
//    }
//    if (ppe_cs_is_empty(from)) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return NULL;
//    }
//
//    frsz = ppe_cs_size(from);
//    tosz = ppe_cs_size(to);
//
//    ret = cs_substitute(s->buf, from, frsz, to, tosz, NULL, &nwsz, n, opt);
//    if (! ret && ppe_err_get_code() != PPE_ERR_TRY_AGAIN) {
//        return NULL;
//    }
//
//    if (nwsz == 1) {
//        return &str_empty_s;
//    }
//
//    nw = (ppe_string) ppe_mp_malloc(sizeof(ppe_string_st) + nwsz - 1); /* The terminating NUL byte has been counted twice. */
//    if (! nw) {
//        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
//        return NULL;
//    }
//
//    ret = cs_substitute(s->buf, from, frsz, to, tosz, nw->buf, &nwsz, n, opt);
//    if (! ret) {
//        ppe_mp_free(nw);
//        return NULL;
//    }
//    nw->sz = nwsz;
//    return nw;
//} /* ppe_str_substitute_cs */
//
///* ==== Definition : String Joiner ========================================= */
//
///* ---- Types --------------------------------------------------------------- */
//
//typedef struct PPE_SJN_JOINER
//{
//    ppe_uint i;     /* The index of the next string to add. */
//    ppe_uint n;     /* Total number of added strings. */
//    ppe_size sz;    /* Total size of appended strings and delimiters. */
//    ppe_size dsz;   /* The length of the delimiter. */
//    ppe_char d[1];  /* The delimiter. */
//} ppe_sjn_joiner_st;
//
///* ---- Functions ----------------------------------------------------------- */
//
///* -- Property -- */
//
//PPE_API ppe_uint ppe_sjn_count(ppe_sjn_joiner_c restrict jnr)
//{
//    return jnr->n;
//} /* ppe_sjn_count */
//
//PPE_API ppe_size ppe_sjn_size(ppe_sjn_joiner_c restrict jnr)
//{
//    return jnr->sz;
//} /* ppe_sjn_size */
//
///* -- Create & Destroy -- */
//
//PPE_API ppe_sjn_joiner ppe_sjn_create(ppe_cstr restrict d, const ppe_size dsz)
//{
//    ppe_sjn_joiner nw = NULL;
//
//    nw = ppe_mp_calloc(sizeof(ppe_sjn_joiner_st) + dsz);
//    if (! nw) {
//        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
//        return NULL;
//    }
//
//    memcpy(nw->d, d, dsz);
//    nw->dsz = dsz;
//    return nw;
//} /* ppe_sjn_create */
//
//PPE_API void ppe_sjn_destroy(ppe_sjn_joiner restrict jnr)
//{
//    ppe_mp_free(jnr);
//} /* ppe_sjn_destroy */
//
//PPE_API void ppe_sjn_reset(ppe_sjn_joiner restrict jnr)
//{
//    jnr->s.addr = NULL;
//    jnr->s.off = 0;
//    jnr->s.sz = 0;
//    jnr->i = 0;
//    jnr->n = 0;
//    jnr->sz = 0;
//} /* ppe_sjn_reset */
//
///* -- Process -- */
//
#define sjn_init_copy(jnr, s, sz) \
    do { \
        jnr->s.addr = s; \
        jnr->s.sz = sz; \
        jnr->s.off = 0; \
    } while (0);

#define sjn_try_copy(jnr, b, bsz, cpsz) \
    do { \
        bytes = jnr->s.sz - jnr->s.off; \
        if (*bsz - cpsz < bytes) { \
            bytes = *bsz - cpsz; \
            memcpy(b + cpsz, jnr->s.addr + jnr->off, bytes); \
            jnr->s.off += bytes; \
            jnr->sz += cpsz + bytes; \
            *bsz = cpsz + bytes; \
            ppe_err_set(PPE_ERR_TRY_AGAIN, NULL); \
            return ppe_false; \
        } else { \
            memcpy(b + cpsz, jnr->s.addr + jnr->off, bytes); \
            jnr->s.off += bytes; \
            cpsz += bytes; \
        } \
    } while (0);

//static void sjn_measure_cstr(ppe_sjn_joiner restrict jnr, ppe_cstr_c restrict s, ppe_size const sz)
//{
//    jnr->sz += sz + (jnr->n == 0 ? 0 : jnr->dsz);
//    jnr->n += 1;
//} /* sjn_measure_cstr */
//
//static ppe_bool sjn_join_one(ppe_sjn_joiner restrict jnr, ppe_cstr_c restrict s, ppe_size const sz, ppe_cstr restrict b, ppe_size * restrict bsz)
//{
//    ppe_size cpsz = 0;
//
//    if (! jnr->s.addr) {
//        /* CASE-1: No string has been copied yet. */
//        sjn_init_copy(jnr, cspt_addr(m), cspt_size(m));
//        sjn_try_copy(jnr, b, bsz, cpsz);
//        *bsz = cpsz;
//        return ppe_true;
//    }
//
//    /* Copy remaining bytes first. */
//    if (jnr->s.off < jnr->sz) {
//        sjn_try_copy(jnr, b, bsz, cpsz);
//        if (jnr->s.addr == s) {
//            *bsz = cpsz;
//            return ppe_true;
//        }
//    } /* if */
//
//    if (jnr->s.addr != jnr->d) {
//        /* CASE-2: The last copied string is a source string. */
//        sjn_init_copy(jnr, jnr->d, jnr->dsz);
//        sjn_try_copy(jnr, b, bsz, cpsz);
//    }
//    sjn_init_copy(jnr, s, sz);
//    sjn_try_copy(jnr, b, bsz, cpsz);
//    *bsz = cpsz;
//    return ppe_true;
//} /* sjn_join_one */
//
//PPE_API ppe_bool ppe_sjn_measure_some(ppe_sjn_joiner restrict jnr, void * restrict ud, ppe_sjn_yield_fn y)
//{
//    ppe_cs_snippet_st spt;
//    ppe_uint idx = 0;
//    ppe_size cpsz = 0;
//    ppe_int ret = 0;
//
//    if (! jnr || ! y) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return ppe_false;
//    }
//
//    cspt_init_on_stack(&spt);
//
//    idx = jnr->i;
//    while ((ret = (*y)(ud, idx, &spt)) == CSPT_PRESERVED_CAPACITY) {
//        cpsz += cspt_total_bytes(spt) + jnr->dsz * CSPT_PRESERVED_CAPACITY;
//        idx += CSPT_PRESERVED_CAPACITY;
//        cspt_reset(spt);
//    } /* while */
//
//    cpsz += cspt_total_bytes(spt) + jnr->dsz * cspt_count(spt);
//    idx += cspt_count(spt);
//
//    jnr->sz += cpsz;
//    if (jnr->n == 0) {
//        /* No string has been measured yet, subtract one delimiter's bytes. */
//        jnr->sz -= jnr->dsz;
//    }
//    jnr->n += (idx - jnr->i);
//
//    if (ret < 0) {
//        jnr->i = idx; /* Keep the last index when any error occurs. */
//        return ppe_false;
//    }
//    jnr->i = 0; /* Reset the index if all strings are measured. */
//    return ppe_true;
//} /* ppe_sjn_measure_some */
//
//PPE_API ppe_bool ppe_sjn_join_some(ppe_sjn_joiner restrict jnr, void * restrict ud, ppe_sjn_yield_fn y, ppe_char * restrict b, ppe_size * restrict bsz)
//{
//    ppe_cs_snippet_st spt;
//    ppe_size cpsz = 0;
//    ppe_size bytes = 0;
//    ppe_uint m = 0;
//    ppe_int ret = 0;
//
//    if (! jnr || ! y || ! b || ! bsz) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return ppe_false;
//    }
//
//    cspt_init_on_stack(&spt);
//
//    /* Copy remaining bytes first. */
//    if (jnr->s.off < jnr->sz) {
//        sjn_try_copy(jnr, b, bsz, cpsz);
//        if (jnr->s.addr != jnr->d) {
//            jnr->i += 1;
//            jnr->n += 1;
//        }
//    } /* if */
//
//    /* Try to get new source strings. */
//    ret = (*y)(ud, jnr->i, &spt);
//    if (ret > 0) {
//        if (jnr->s.addr == jnr->d || ! jnr->s.addr) {
//            /* CASE-1: The last copied string is the delimiter. */
//            /* CASE-2: No string has been copied. */
//            sjn_init_copy(jnr, cspt_addr(m), cspt_size(m));
//            sjn_try_copy(jnr, b, bsz, cpsz);
//            jnr->i += 1;
//            jnr->n += 1;
//            m += 1;
//            ret -= 1;
//        } /* if */
//
//        /* CASE-3: The last copied string is a source string. */
//
//        do {
//            switch (ret) {
//#if (CSPT_PRESERVED_CAPACITY == 8)
//                case 8:
//                    sjn_init_copy(jnr, jnr->d, jnr->dsz);
//                    sjn_try_copy(jnr, b, bsz, cpsz);
//                    sjn_init_copy(jnr, cspt_addr(m), cspt_size(m));
//                    sjn_try_copy(jnr, b, bsz, cpsz);
//                    jnr->i += 1;
//                    jnr->n += 1;
//                    m += 1;
//                case 7:
//                    sjn_init_copy(jnr, jnr->d, jnr->dsz);
//                    sjn_try_copy(jnr, b, bsz, cpsz);
//                    sjn_init_copy(jnr, cspt_addr(m), cspt_size(m));
//                    sjn_try_copy(jnr, b, bsz, cpsz);
//                    jnr->i += 1;
//                    jnr->n += 1;
//                    m += 1;
//                case 6:
//                    sjn_init_copy(jnr, jnr->d, jnr->dsz);
//                    sjn_try_copy(jnr, b, bsz, cpsz);
//                    sjn_init_copy(jnr, cspt_addr(m), cspt_size(m));
//                    sjn_try_copy(jnr, b, bsz, cpsz);
//                    jnr->i += 1;
//                    jnr->n += 1;
//                    m += 1;
//                case 5:
//                    sjn_init_copy(jnr, jnr->d, jnr->dsz);
//                    sjn_try_copy(jnr, b, bsz, cpsz);
//                    sjn_init_copy(jnr, cspt_addr(m), cspt_size(m));
//                    sjn_try_copy(jnr, b, bsz, cpsz);
//                    jnr->i += 1;
//                    jnr->n += 1;
//                    m += 1;
//#endif
//                case 4:
//                    sjn_init_copy(jnr, jnr->d, jnr->dsz);
//                    sjn_try_copy(jnr, b, bsz, cpsz);
//                    sjn_init_copy(jnr, cspt_addr(m), cspt_size(m));
//                    sjn_try_copy(jnr, b, bsz, cpsz);
//                    jnr->i += 1;
//                    jnr->n += 1;
//                    m += 1;
//                case 3:
//                    sjn_init_copy(jnr, jnr->d, jnr->dsz);
//                    sjn_try_copy(jnr, b, bsz, cpsz);
//                    sjn_init_copy(jnr, cspt_addr(m), cspt_size(m));
//                    sjn_try_copy(jnr, b, bsz, cpsz);
//                    jnr->i += 1;
//                    jnr->n += 1;
//                    m += 1;
//                case 2:
//                    sjn_init_copy(jnr, jnr->d, jnr->dsz);
//                    sjn_try_copy(jnr, b, bsz, cpsz);
//                    sjn_init_copy(jnr, cspt_addr(m), cspt_size(m));
//                    sjn_try_copy(jnr, b, bsz, cpsz);
//                    jnr->i += 1;
//                    jnr->n += 1;
//                    m += 1;
//                case 1:
//                    sjn_init_copy(jnr, jnr->d, jnr->dsz);
//                    sjn_try_copy(jnr, b, bsz, cpsz);
//                    sjn_init_copy(jnr, cspt_addr(m), cspt_size(m));
//                    sjn_try_copy(jnr, b, bsz, cpsz);
//                    jnr->i += 1;
//                    jnr->n += 1;
//                    m += 1;
//                case 0:
//                    break;
//
//                default:
//                    ppe_err_set(PPE_ERR_OUT_OF_RANGE, "The number of joining strings must be 4 or less");
//                    return ppe_false;
//            } /* switch */
//
//            /* Try to get new source strings. */
//            cspt_reset(&spt);
//            m = 0;
//        } while (m == CSPT_PRESERVED_CAPACITY && (ret = (*y)(ud, jnr->i, &spt)) > 0);
//    } /* if */
//
//    jnr->sz += cpsz;
//    *bsz = cpsz;
//    if (ret < 0) {
//        return ppe_false;
//    } /* if */
//    jnr->i = 0;
//    return ppe_true;
//} /* ppe_sjn_join_some */
//
//PPE_API ppe_bool ppe_sjn_measure_cstr(ppe_sjn_joiner restrict jnr, ppe_cstr_c restrict s, ppe_ssize const sz)
//{
//    if (! jnr || ! s) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return ppe_false;
//    }
//    sjn_measure_cstr(jnr, s, (sz < 0L ? ppe_cs_size(s) : (ppe_size)sz));
//    return ppe_true;
//} /* ppe_sjn_measure_cstr */
//
//PPE_API ppe_bool ppe_sjn_join_one_cstr(ppe_sjn_joiner restrict jnr, ppe_cstr_c restrict s, ppe_ssize const sz, ppe_cstr restrict b, ppe_size * restrict bsz)
//{
//    if (! jnr || ! s || ! b || ! bsz) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return ppe_false;
//    }
//    return sjn_join_one(jnr, s, (sz < 0 ? ppe_cs_size(s) : (ppe_size)sz), b, bsz);
//} /* ppe_sjn_join_one_cstr */
//
//PPE_API ppe_bool ppe_sjn_measure(ppe_sjn_joiner restrict jnr, ppe_string restrict s)
//{
//    if (! jnr || ! s) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return ppe_false;
//    }
//    sjn_measure_cstr(jnr, ppe_str_addr(s), ppe_str_size(s));
//    return ppe_true;
//} /* ppe_sjn_measure */
//
//PPE_API ppe_bool ppe_sjn_join_one_string(ppe_sjn_joiner restrict jnr, ppe_string restrict s, ppe_cstr restrict b, ppe_size * restrict bsz)
//{
//    if (! jnr || ! s || ! b || ! bsz) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return ppe_false;
//    }
//    return sjn_join_one(jnr, ppe_str_addr(s), ppe_str_size(s), b, bsz);
//} /* ppe_sjn_join_one_string */
//
//typedef struct
//{
//    ppe_cstr_c * strs;
//    ppe_uint n;
//} sjn_cstr_array_st, *sjn_cstr_array;
//
//static ppe_int sjn_yield_cstr_from_array(void * ud, const ppe_uint idx, ppe_cs_snippet spt)
//{
//    sjn_cstr_array d = (sjn_cstr_array) ud;
//    ppe_uint i = 0;
//    ppe_uint m = 0;
//    ppe_uint cnt = 0;
//
//    if (idx >= d->n) {
//        return 0;
//    } 
//
//    cnt = cspt_unused(spt);
//    if (d->n - idx < cnt) {
//        cnt = d->n - idx;
//    }
//    i = cnt;
//    m = idx;
//    while (i >= 4) {
//        cspt_append(spt, d->str[m], ppe_cs_size(d->strs[m]));
//        m += 1;
//        cspt_append(spt, d->str[m], ppe_cs_size(d->strs[m]));
//        m += 1;
//        cspt_append(spt, d->str[m], ppe_cs_size(d->strs[m]));
//        m += 1;
//        cspt_append(spt, d->str[m], ppe_cs_size(d->strs[m]));
//        m += 1;
//        i -= 4;
//    } /* while */
//
//    if (i & 0x2) {
//        cspt_append(spt, d->str[m], ppe_cs_size(d->strs[m]));
//        m += 1;
//        cspt_append(spt, d->str[m], ppe_cs_size(d->strs[m]));
//        m += 1;
//    } /* if */
//    if (i & 0x1) {
//        cspt_append(spt, d->str[m], ppe_cs_size(d->strs[m]));
//        m += 1;
//    } /* if */
//    return cnt;
//} /* sjn_yield_cstr_from_array */
//
//PPE_API ppe_bool ppe_sjn_measure_cstr_array(ppe_sjn_joiner restrict jnr, ppe_cstr_c * restrict strs, const ppe_uint n)
//{
//    ppe_uint i = 0;
//
//    if (! strs || n == 0) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return ppe_false;
//    }
//
//    for (; i < n; i += 1) {
//        sjn_measure_cstr(jnr, strs[i], ppe_cs_size(strs[i]));
//    } /* for */
//    return ppe_true;
//} /* ppe_sjn_measure_cstr_array */
//
//PPE_API ppe_bool ppe_sjn_join_some_cstrs(ppe_sjn_joiner restrict jnr, ppe_cstr_c * restrict strs, const ppe_uint n, ppe_cstr restrict b, ppe_size * restrict bsz)
//{
//    sjn_cstr_array_st ud;
//
//    if (! strs || n == 0) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return ppe_false;
//    }
//
//    ud.c = strs;
//    ud.n = n;
//    return ppe_sjn_join_some(jnr, &ud, &sjn_yield_cstr_from_array, b, bsz);
//} /* ppe_sjn_join_some_cstrs */
//
//typedef struct
//{
//    ppe_string_c * strs;
//    ppe_uint n;
//} sjn_string_array_st, *sjn_string_array;
//
//static ppe_int sjn_yield_cstr_from_string_array(void * ud, const ppe_uint idx, ppe_cs_snippet restrict spt)
//{
//    sjn_string_array d = (sjn_string_array) ud;
//    ppe_uint i = 0;
//    ppe_uint m = 0;
//    ppe_uint cnt = 0;
//
//    if (idx >= d->n) {
//        return 0;
//    } 
//
//    cnt = cspt_unused(spt);
//    if (d->n - idx < cnt) {
//        cnt = d->n - idx;
//    }
//    i = cnt;
//    m = idx;
//    while (i >= 4) {
//        cspt_append(spt, ppe_str_addr(d->strs[m]), ppe_str_size(d->strs[m]));
//        m += 1;
//        cspt_append(spt, ppe_str_addr(d->strs[m]), ppe_str_size(d->strs[m]));
//        m += 1;
//        cspt_append(spt, ppe_str_addr(d->strs[m]), ppe_str_size(d->strs[m]));
//        m += 1;
//        cspt_append(spt, ppe_str_addr(d->strs[m]), ppe_str_size(d->strs[m]));
//        m += 1;
//        i -= 4;
//    } /* while */
//
//    if (cnt & 0x2) {
//        cspt_append(spt, ppe_str_addr(d->strs[m]), ppe_str_size(d->strs[m]));
//        m += 1;
//        cspt_append(spt, ppe_str_addr(d->strs[m]), ppe_str_size(d->strs[m]));
//        m += 1;
//    } /* if */
//    if (cnt & 0x1) {
//        cspt_append(spt, ppe_str_addr(d->strs[m]), ppe_str_size(d->strs[m]));
//        m += 1;
//    } /* if */
//    return cnt;
//} /* sjn_yield_cstr_from_string_array */
//
//PPE_API ppe_bool ppe_sjn_measure_array(ppe_sjn_joiner restrict jnr, ppe_string_c * restrict strs, const ppe_uint n)
//{
//    ppe_uint i = 0;
//
//    if (! jnr || ! strs) {
//        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
//        return ppe_false;
//    }
//
//    for (; i < n; i += 1) {
//        sjn_measure_cstr(jnr, ppe_str_addr(strs[i]), ppe_str_size(strs[i]));
//    } /* for */
//    return ppe_true;
//} /* ppe_sjn_measure_array */
//
//PPE_API ppe_bool ppe_sjn_join_some_strings(ppe_sjn_joiner restrict jnr, ppe_string_c * restrict strs, const ppe_uint n, ppe_cstr restrict b, ppe_size * restrict bsz)
//{
//    sjn_string_array_st ud;
//
//    ud.c = strs;
//    ud.n = n;
//    return ppe_sjn_join_some(jnr, &ud, &sjn_yield_cstr_from_string_array, b, bsz);
//} /* ppe_sjn_join_some_strings */
//
//static ppe_int sjn_yield_cstr_from_snippet(void * ud, const ppe_uint idx, const ppe_char ** s, const ppe_size * sz, const ppe_uint n)
//{
//    ppe_cs_snippet spt = (ppe_cs_snippet) ud;
//    ppe_uint i = 0;
//    ppe_uint m = 0;
//    ppe_uint cnt = 0;
//
//    if (idx >= d->n) {
//        return 0;
//    } 
//
//    cnt = cspt_unused(spt);
//    if (d->n - idx < cnt) {
//        cnt = d->n - idx;
//    }
//    i = cnt;
//    m = idx;
//    while (i >= 4) {
//        cspt_append(spt, cspt_addr(spt, m), cspt_size(spt, m));
//        m += 1;
//        cspt_append(spt, cspt_addr(spt, m), cspt_size(spt, m));
//        m += 1;
//        cspt_append(spt, cspt_addr(spt, m), cspt_size(spt, m));
//        m += 1;
//        cspt_append(spt, cspt_addr(spt, m), cspt_size(spt, m));
//        m += 1;
//        i -= 4;
//    } /* while */
//
//    if (c & 0x2) {
//        cspt_append(spt, cspt_addr(spt, m), cspt_size(spt, m));
//        m += 1;
//        cspt_append(spt, cspt_addr(spt, m), cspt_size(spt, m));
//        m += 1;
//    } /* if */
//    if (c & 0x1) {
//        cspt_append(spt, cspt_addr(spt, m), cspt_size(spt, m));
//        m += 1;
//    } /* if */
//    return cnt;
//} /* sjn_yield_cstr_from_snippet */
//
//PPE_API ppe_bool ppe_sjn_measure_some_snippets(ppe_sjn_joiner restrict jnr, ppe_cs_snippet restrict spt)
//{
//    return ppe_sjn_measure_some(jnr, spt, &sjn_yield_cstr_from_snippet);
//} /* ppe_sjn_measure_some_snippets */
//
//PPE_API ppe_bool ppe_sjn_join_some_snippets(ppe_sjn_joiner restrict jnr, ppe_cs_snippet restrict spt, ppe_cstr restrict b, ppe_size * restrict bsz)
//{
//    return ppe_sjn_join_some(jnr, spt, &sjn_yield_cstr_from_snippet, b, bsz);
//} /* ppe_sjn_join_some_snippets */

#ifdef __cplusplus
}
#endif

