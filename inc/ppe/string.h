#ifndef PPE_STRING_H
#define PPE_STRING_H 1 

#include <stdarg.h>

#include "ppe/types.h"
#include "ppe/macros.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ==== Declaration : String ==== */

/* ---- Types ---- */

struct ppe_string_st; 
typedef struct ppe_string_st * ppe_string;

typedef struct PPE_TEXT
{
    ppe_size len;
    const char * buf;
} ppe_text_st, *ppe_text;

/* ---- Macros ---- */

#define PPE_STR_DETECT_LENGTH (~ (ppe_size)0L)
#define PPE_STR_INVALID_LENGTH (~ (ppe_size)0L)
#define PPE_STR_MAX_LENGTH ((~ (ppe_size)0L) / 2)

#define PPE_STR_ARG_END (~ (void*)0)

#define PPE_STR_MAX_INPUT_N 32

/* ---- Interfaces ---- */

/* ---- Functions ---- */

/* -- Preset values -- */

PPE_API extern const ppe_string ppe_str_get_empty(void);

/* -- Create & Destroy -- */

/* _cs_ series */

PPE_API extern ppe_string ppe_cs_clone(const ppe_char * restrict src, const ppe_size len);

/* split */

PPE_API extern ppe_string * ppe_cs_split(const char * restrict d, const ppe_size dsz, const ppe_string restrict s, ppe_uint * restrict n);
PPE_API extern ppe_string * ppe_cs_split_cstr(const char * restrict d, const ppe_size dsz, const char * restrict s, const ppe_size sz, ppe_uint * restrict n);

/* join */

PPE_API extern ppe_string ppe_cs_join_var(const char * restrict deli, const ppe_size deli_len, const ppe_string restrict s1, const ppe_string restrict s2, va_list args);
PPE_API extern ppe_string ppe_cs_join_list(const char * restrict deli, const ppe_size deli_len, const ppe_string * restrict srcs, const ppe_uint n);

PPE_API extern ppe_string ppe_cs_join_cstr_var(const char * restrict deli, const ppe_size deli_len, const char * restrict s1, const char * restrict s2, va_list args);
PPE_API extern ppe_string ppe_cs_join_cstr_list(const char * restrict deli, const ppe_size deli_len, const char ** restrict srcs, const ppe_uint n);

PPE_API extern ppe_string ppe_cs_join_text_var(const char * restrict deli, const ppe_size deli_len, const char * restrict s1, const ppe_size sz1, const char * restrict s2, const ppe_size sz2, va_list args);
PPE_API extern ppe_string ppe_cs_join_text_list(const char * restrict deli, const ppe_size deli_len, const ppe_text_st * restrict srcs, const ppe_uint n);

PPE_API extern ppe_string ppe_cs_join_2(const char * restrict deli, const ppe_size deli_len, const ppe_size deli_len, const char * restrict s1, const ppe_size sz1, const char * restrict s2, const ppe_size sz2);

/* _str_ series */

PPE_API extern ppe_string ppe_str_clone(const ppe_string restrict src);

/* split */

PPE_API extern ppe_string * ppe_str_split(const ppe_string restrict d, const ppe_string restrict s, ppe_uint * restrict n);
PPE_API extern ppe_string * ppe_str_split_cstr(const ppe_string restrict d, const char * restrict s, const ppe_size sz, ppe_uint * restrict n);

/* join */

PPE_API extern ppe_string ppe_str_join_var(const ppe_string restrict deli, const ppe_string restrict s1, const ppe_string restrict s2, va_list args);
PPE_API extern ppe_string ppe_str_join_list(const ppe_string restrict deli, const ppe_string * restrict srcs, const ppe_uint n);

PPE_API extern ppe_string ppe_str_join_cstr_var(const ppe_string restrict deli, const char * restrict s1, const char * restrict s2, va_list args);
PPE_API extern ppe_string ppe_str_join_cstr_list(const ppe_string restrict deli, const char ** restrict srcs, const ppe_uint n);

PPE_API extern ppe_string ppe_str_join_text_var(const ppe_string restrict deli, const char * restrict s1, const ppe_size sz1, const char * restrict s2, const ppe_size sz2, va_list args);
PPE_API extern ppe_string ppe_str_join_text_list(const ppe_string restrict deli, const ppe_text_st * restrict srcs, const ppe_uint n);

PPE_API extern ppe_string ppe_str_join_2(const ppe_string restrict deli, const ppe_string restrict s1, const ppe_string restrict s2);

/* destroy */

PPE_API extern void ppe_str_destroy(ppe_string restrict src);

/* -- Formatting -- */

/* PPE_API extern ppe_size ppe_misc_snprintf(char * restrict buf, ppe_size * restrict buf_size, const char * restrict format, ...); */
PPE_API extern ppe_string ppe_cs_vsprintf(const char * restrict fmt, va_list args);

/* -- Property -- */

PPE_API extern const char * ppe_str_cstr(ppe_string restrict str);
PPE_API extern ppe_size ppe_str_size(ppe_string restrict str);

/* -- Comparison -- */

PPE_API extern ppe_bool ppe_str_equals(const ppe_string restrict str1, const ppe_string restrict str2);
PPE_API extern ppe_bool ppe_str_less_than(const ppe_string restrict str1, const ppe_string restrict str2);
PPE_API extern ppe_bool ppe_str_greater_than(const ppe_string restrict str1, const ppe_string restrict str2);

/* -- Wrapper -- */

/* _cs_ series */

static inline ppe_string ppe_cs_join_cstr(const char * restrict deli, const ppe_size deli_len, const char * restrict s1, const char * restrict s2, ...)
{
    ppe_string new_str = NULL;
    va_list args;

    va_start(args, s2);
    new_str = ppe_cs_join_cstr_var(deli, deli_len, s1, sz1, s2, sz2, args);
    va_end(args);
    return new_str;
}

static inline ppe_string ppe_cs_join_text(const char * restrict deli, const ppe_size deli_len, const char * restrict s1, const ppe_size sz1, const char * restrict s2, const ppe_size sz2, ...)
{
    ppe_string new_str = NULL;
    va_list args;

    va_start(args, sz2);
    new_str = ppe_cs_join_text_var(deli, deli_len, s1, sz1, s2, sz2, args);
    va_end(args);
    return new_str;
}

static inline ppe_string ppe_cs_join_string(const char * restrict deli, const ppe_size deli_len, const ppe_string restrict s1, const ppe_string restrict s2, ...)
{
    ppe_string new_str = NULL;
    va_list args;

    va_start(args, s2);
    new_str = ppe_cs_join_var(deli, deli_len, s1, s2, args);
    va_end(args);
    return new_str;
}

static inline ppe_string ppe_cs_concat(const char * restrict s1, const char * restrict s2, ...)
{
    ppe_string new_str = NULL;
    va_list args;

    va_start(args, s2);
    new_str = ppe_cs_join_cstr_var("", 0, s1, sz1, s2, sz2, args);
    va_end(args);
    return new_str;
}

static inline ppe_string ppe_cs_concat_text(const char * restrict s1, const ppe_size sz1, const char * restrict s2, const ppe_size sz2, ...)
{
    ppe_string new_str = NULL;
    va_list args;

    va_start(args, sz2);
    new_str = ppe_cs_join_text_var("", 0, s1, sz1, s2, sz2, args);
    va_end(args);
    return new_str;
}
static inline ppe_string ppe_cs_concat_list(ppe_text_st * restrict srcs, const ppe_uint n)
{
    return ppe_cs_join_cstr_list("", 0, srcs, n);
}

static inline ppe_string ppe_cs_concat_2(const char * restrict s1, const ppe_size sz1, const char * restrict s2, const ppe_size sz2)
{
    return ppe_cs_join_2("", 0, s1, sz1, s2, sz2);
}

static ppe_string ppe_cs_sprintf(const char * restrict fmt, ...)
{
    ppe_string new_str = NULL;
    va_list args;

    va_start(args, fmt);
    new_str = ppe_cs_vsprintf(fmt, args);
    va_end(args);
    return new_str;
}

/* _str_ series */

static inline ppe_string ppe_str_join_cstr(const ppe_string restrict deli, const char * restrict s1, const char * restrict s2, ...)
{
    ppe_string new_str = NULL;
    va_list args;

    va_start(args, s2);
    new_str = ppe_str_join_cstr_var(deli, s1, sz1, s2, sz2, args);
    va_end(args);
    return new_str;
}

static inline ppe_string ppe_str_join_text(const ppe_string restrict deli, const char * restrict s1, const ppe_size sz1, const char * restrict s2, const ppe_size sz2, ...)
{
    ppe_string new_str = NULL;
    va_list args;

    va_start(args, sz2);
    new_str = ppe_str_join_text_var(deli, s1, sz1, s2, sz2, args);
    va_end(args);
    return new_str;
}

static inline ppe_string ppe_str_join(const ppe_string restrict deli, const ppe_string restrict s1, const ppe_string restrict s2, ...)
{
    ppe_string new_str = NULL;
    va_list args;

    va_start(args, s2);
    new_str = ppe_str_join_var(deli, s1, s2, args);
    va_end(args);
    return new_str;
}

static inline ppe_string ppe_str_concat(const ppe_string restrict s1, const ppe_string restrict s2, ...)
{
    ppe_string new_str = NULL;
    va_list args;

    va_start(args, s2);
    new_str = ppe_cs_join_var("", 0, s1, s2, args);
    va_end(args);
    return new_str;
}

static inline ppe_string ppe_str_concat_list(const ppe_string * restrict srcs, const ppe_uint n)
{
    return ppe_cs_join_cstr_list("", 0, srcs, n);
}

static inline ppe_string ppe_str_concat_2(const ppe_string restrict s1, const ppe_string restrict s2)
{
    return ppe_cs_join_2("", 0, s1, s2, args);
}

/* ==== Declaration : String Tracer ==== */

/* ---- Types ---- */

struct ppe_str_tracer_st;
typedef struct ppe_str_tracer_st * ppe_str_tracer;

/* ---- Preset Values ---- */

#define PPE_STC_ALL ((ppe_uint) 0L)

/* ---- Functions ---- */

/* -- Create & Destroy -- */

PPE_API extern ppe_str_tracer ppe_stc_create(const ppe_string restrict s);
PPE_API extern ppe_str_tracer ppe_stc_create_for_cstr(const char * restrict s, const ppe_size sz);

PPE_API extern ppe_bool ppe_stc_reset(ppe_str_tracer restrict trc, const ppe_string restrict s);
PPE_API extern ppe_bool ppe_stc_reset_for_cstr(ppe_str_tracer restrict trc, const char * restrict s, const ppe_size sz);

PPE_API extern void ppe_stc_destroy(ppe_str_tracer restrict trc);

/* -- Finding -- */

PPE_API extern ppe_int ppe_stc_find(ppe_str_tracer restrict trc, const ppe_string restrict s, const ppe_uint n);
PPE_API extern ppe_int ppe_stc_find_cstr(ppe_str_tracer restrict trc, const char * restrict s, const ppe_size sz, const ppe_uint n);
PPE_API extern ppe_bool ppe_stc_continue(ppe_str_tracer restrict trc);

/* -- Split -- */

PPE_API extern ppe_string ppe_stc_split_one(ppe_str_tracer restrict trc, const ppe_uint idx);
PPE_API extern ppe_string * ppe_stc_split(ppe_str_tracer restrict trc, const ppe_uint idx, const ppe_uint * restrict n);

/* -- Replace -- */

PPE_API extern ppe_string ppe_stc_replace_by_joiner(ppe_str_tracer restrict trc, const ppe_uint idx, const ppe_uint n, ppe_str_joiner restrict jnr);
PPE_API extern ppe_string ppe_stc_replace_with_cstr(ppe_str_tracer restrict trc, const ppe_uint idx, const ppe_uint n, const char * restrict s, const ppe_size sz);
PPE_API extern ppe_string ppe_stc_replace(ppe_str_tracer restrict trc, const ppe_uint idx, const ppe_int cnt, const ppe_string restrict s);

/* -- Remove -- */

PPE_API extern ppe_string ppe_stc_remove(ppe_str_tracer restrict trc, const ppe_uint idx, const ppe_uint n);

/* -- Property -- */

PPE_API extern ppe_int ppe_stc_count(ppe_str_tracer restrict trc);

/* -- Wrapper -- */
static inline ppe_string * ppe_stc_split_all(ppe_str_tracer restrict trc, ppe_uint * restrict cnt)
{
    ppe_uint c = ppe_stc_count(trc) + 1;
    ppe_string * arr = ppe_stc_split(trc, 0, &c);
    if (arr && cnt) {
        *cnt = c;
    }
    return arr;
}

static inline ppe_int ppe_stc_last_index(ppe_str_tracer restrict trc)
{
    ppe_int idx = ppe_stc_count(trc);
    return (idx > 0) ? idx - 1 : idx;
}

/* ==== Declaration : String Joiner ==== */

/* ---- Types ---- */

struct ppe_str_joiner_st;
typedef struct ppe_str_joiner_st * ppe_str_joiner;

/* ---- Functions ---- */

/* -- Create & Destroy -- */

PPE_API extern ppe_str_joiner ppe_sjn_create_from_cstr(const char * restrict d);
PPE_API extern ppe_str_joiner ppe_sjn_create_from(const char * restrict d);

PPE_API extern void ppe_sjn_destroy(ppe_str_joiner restrict jnr);
PPE_API extern void ppe_sjn_reset(ppe_str_joiner restrict jnr);

PPE_API extern ppe_bool ppe_sjn_refer_to_cstr(ppe_str_joiner restrict jnr, const char * restrict s, const ppe_size sz);
PPE_API extern ppe_bool ppe_sjn_refer_to(ppe_str_joiner restrict jnr, const ppe_string restrict s);

PPE_API extern ppe_bool ppe_sjn_copy_from_cstr(ppe_str_joiner restrict jnr, const char * restrict s, const ppe_size sz);
PPE_API extern ppe_bool ppe_sjn_copy_from(ppe_str_joiner restrict jnr, const ppe_string restrict s);

PPE_API extern ppe_string ppe_sjn_join(ppe_str_joiner restrict jnr, const ppe_bool reset);

/* ==== Declaration : String Bunch ==== */

/* ---- Types ---- */

struct ppe_str_bunch_st;
typedef struct ppe_str_bunch_st * ppe_str_bunch;

/* ---- Functions ---- */

PPE_API extern ppe_str_bunch ppe_sbc_create(void);
PPE_API extern void ppe_sbc_destroy(ppe_str_bunch restrict bc);
PPE_API extern void ppe_sbc_reset(ppe_str_bunch restrict bc);

PPE_API extern ppe_bool ppe_sbc_refer_to_cstr(ppe_str_bunch restrict bc, const char * restrict s, const ppe_size sz);
PPE_API extern ppe_bool ppe_sbc_refer_to(ppe_str_bunch restrict bc, const ppe_string restrict s);

PPE_API extern ppe_bool ppe_sbc_copy_from_cstr(ppe_str_bunch restrict bc, const char * restrict s, const ppe_size sz);
PPE_API extern ppe_bool ppe_sbc_copy_from(ppe_str_bunch restrict bc, const ppe_string restrict s);

PPE_API extern ppe_string ppe_sbc_join_by_cstr(ppe_str_bunch restrict bc, const char * restrict s, const ppe_size sz);
PPE_API extern ppe_string ppe_sbc_join_by(ppe_str_bunch restrict bc, const ppe_string restrict s);

PPE_API extern ppe_string ppe_sbc_concat(ppe_str_bunch restrict bc);

PPE_API extern ppe_bool ppe_sbc_reference(ppe_str_bunch restrict bc, const ppe_uint idx, const char ** restrict s, const ppe_size * restrict sz);
PPE_API extern ppe_uint ppe_sbc_count(ppe_str_bunch restrict bc);

#ifdef __cplusplus
}
#endif

#endif /* PPE_STRING_H */