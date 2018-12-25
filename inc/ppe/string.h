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

PPE_API extern ppe_string ppe_cs_join_cstr_var(const char * restrict deli, const ppe_size deli_len, const char * restrict src1, const char * restrict src2, va_list args);
PPE_API extern ppe_string ppe_cs_join_cstr_list(const char * restrict deli, const ppe_size deli_len, const char ** restrict srcs, const ppe_uint n);

PPE_API extern ppe_string ppe_cs_join_text_var(const char * restrict deli, const ppe_size deli_len, const char * restrict src1, const ppe_size len1, const char * restrict src2, const ppe_size len2, va_list args);
PPE_API extern ppe_string ppe_cs_join_text_list(const char * restrict deli, const ppe_size deli_len, const ppe_text_st * restrict srcs, const ppe_uint n);

PPE_API extern ppe_string ppe_cs_join_string_var(const char * restrict deli, const ppe_size deli_len, const ppe_string restrict src1, const ppe_string restrict src2, va_list args);
PPE_API extern ppe_string ppe_cs_join_string_list(const char * restrict deli, const ppe_size deli_len, const ppe_string * restrict srcs, const ppe_uint n);

PPE_API extern ppe_string ppe_cs_join_2(const char * restrict deli, const ppe_size deli_len, const ppe_size deli_len, const char * restrict src1, const ppe_size len1, const char * restrict src2, const ppe_size len2);

/* PPE_API extern ppe_size ppe_misc_snprintf(char * restrict buf, ppe_size * restrict buf_size, const char * restrict format, ...); */
PPE_API extern ppe_string ppe_cs_vsprintf(const char * restrict fmt, va_list args);

/* _str_ series */

PPE_API extern ppe_string ppe_str_clone(const ppe_string restrict src);

PPE_API extern ppe_string ppe_str_join_cstr_var(const ppe_string restrict deli, const char * restrict src1, const char * restrict src2, va_list args);
PPE_API extern ppe_string ppe_str_join_cstr_list(const ppe_string restrict deli, const char ** restrict srcs, const ppe_uint n);

PPE_API extern ppe_string ppe_str_join_text_var(const ppe_string restrict deli, const char * restrict src1, const ppe_size len1, const char * restrict src2, const ppe_size len2, va_list args);
PPE_API extern ppe_string ppe_str_join_text_list(const ppe_string restrict deli, const ppe_text_st * restrict srcs, const ppe_uint n);

PPE_API extern ppe_string ppe_str_join_string_var(const ppe_string restrict deli, const ppe_string restrict src1, const ppe_string restrict src2, va_list args);
PPE_API extern ppe_string ppe_str_join_string_list(const ppe_string restrict deli, const ppe_string * restrict srcs, const ppe_uint n);

PPE_API extern ppe_string ppe_str_join_2(const ppe_string restrict deli, const ppe_string restrict src1, const ppe_string restrict src2);

PPE_API extern void ppe_str_destroy(ppe_string restrict src);

/* -- Property -- */

PPE_API extern const char * ppe_str_cstr(ppe_string restrict str);
PPE_API extern ppe_size ppe_str_size(ppe_string restrict str);

/* -- Comparison -- */

PPE_API extern ppe_bool ppe_str_equals(const ppe_string restrict str1, const ppe_string restrict str2);
PPE_API extern ppe_bool ppe_str_less_than(const ppe_string restrict str1, const ppe_string restrict str2);
PPE_API extern ppe_bool ppe_str_greater_than(const ppe_string restrict str1, const ppe_string restrict str2);

/* -- Wrapper -- */

/* _cs_ series */

static inline ppe_string ppe_cs_join_cstr(const char * restrict deli, const ppe_size deli_len, const char * restrict src1, const char * restrict src2, ...)
{
    ppe_string new_str = NULL;
    va_list args;

    va_start(args, src2);
    new_str = ppe_cs_join_cstr_var(deli, deli_len, src1, len1, src2, len2, args);
    va_end(args);
    return new_str;
}

static inline ppe_string ppe_cs_join_text(const char * restrict deli, const ppe_size deli_len, const char * restrict src1, const ppe_size len1, const char * restrict src2, const ppe_size len2, ...)
{
    ppe_string new_str = NULL;
    va_list args;

    va_start(args, len2);
    new_str = ppe_cs_join_text_var(deli, deli_len, src1, len1, src2, len2, args);
    va_end(args);
    return new_str;
}

static inline ppe_string ppe_cs_join_string(const char * restrict deli, const ppe_size deli_len, const ppe_string restrict src1, const ppe_string restrict src2, ...)
{
    ppe_string new_str = NULL;
    va_list args;

    va_start(args, src2);
    new_str = ppe_cs_join_string_var(deli, deli_len, src1, src2, args);
    va_end(args);
    return new_str;
}

static inline ppe_string ppe_cs_concat(const char * restrict src1, const char * restrict src2, ...)
{
    ppe_string new_str = NULL;
    va_list args;

    va_start(args, src2);
    new_str = ppe_cs_join_cstr_var("", 0, src1, len1, src2, len2, args);
    va_end(args);
    return new_str;
}

static inline ppe_string ppe_cs_concat_text(const char * restrict src1, const ppe_size len1, const char * restrict src2, const ppe_size len2, ...)
{
    ppe_string new_str = NULL;
    va_list args;

    va_start(args, len2);
    new_str = ppe_cs_join_text_var("", 0, src1, len1, src2, len2, args);
    va_end(args);
    return new_str;
}
static inline ppe_string ppe_cs_concat_list(ppe_text_st * restrict srcs, const ppe_uint n)
{
    return ppe_cs_join_list("", 0, srcs, n);
}

static inline ppe_string ppe_cs_concat_2(const char * restrict src1, const ppe_size len1, const char * restrict src2, const ppe_size len2)
{
    return ppe_cs_join_2("", 0, src1, len1, src2, len2);
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

static inline ppe_string ppe_str_join_cstr(const ppe_string restrict deli, const char * restrict src1, const char * restrict src2, ...)
{
    ppe_string new_str = NULL;
    va_list args;

    va_start(args, src2);
    new_str = ppe_str_join_cstr_var(deli, src1, len1, src2, len2, args);
    va_end(args);
    return new_str;
}

static inline ppe_string ppe_str_join_text(const ppe_string restrict deli, const char * restrict src1, const ppe_size len1, const char * restrict src2, const ppe_size len2, ...)
{
    ppe_string new_str = NULL;
    va_list args;

    va_start(args, len2);
    new_str = ppe_str_join_text_var(deli, src1, len1, src2, len2, args);
    va_end(args);
    return new_str;
}

static inline ppe_string ppe_str_join_string(const ppe_string restrict deli, const ppe_string restrict src1, const ppe_string restrict src2, ...)
{
    ppe_string new_str = NULL;
    va_list args;

    va_start(args, src2);
    new_str = ppe_str_join_string_var(deli, src1, src2, args);
    va_end(args);
    return new_str;
}

static inline ppe_string ppe_str_concat(const ppe_string restrict src1, const ppe_string restrict src2, ...)
{
    ppe_string new_str = NULL;
    va_list args;

    va_start(args, src2);
    new_str = ppe_cs_join_string_var("", 0, src1, src2, args);
    va_end(args);
    return new_str;
}

static inline ppe_string ppe_str_concat_list(const ppe_string * restrict srcs, const ppe_uint n)
{
    return ppe_cs_join_list("", 0, srcs, n);
}

static inline ppe_string ppe_str_concat_2(const ppe_string restrict src1, const ppe_string restrict src2)
{
    return ppe_cs_join_2("", 0, src1, src2, args);
}

/* ==== Declaration : String Joiner ==== */

/* ---- Types ---- */

struct ppe_str_joiner_st;
typedef struct ppe_str_joiner_st * ppe_str_joiner;

/* ---- Interfaces ---- */

/* ---- Functions ---- */

/* -- Create & Destroy -- */

PPE_API extern ppe_str_joiner ppe_sjn_create(const char * restrict deli);
PPE_API extern void ppe_sjn_destroy(ppe_str_joiner restrict jnr);
PPE_API extern void ppe_sjn_reset(ppe_str_joiner restrict jnr);

PPE_API extern ppe_bool ppe_sjn_append(ppe_str_joiner restrict jnr, const ppe_string restrict src);
PPE_API extern ppe_bool ppe_sjn_append_cstr(ppe_str_joiner restrict jnr, const char * restrict src, const ppe_size len);
PPE_API extern ppe_string ppe_sjn_join(ppe_str_joiner restrict jnr, ppe_bool reset);

#ifdef __cplusplus
}
#endif

#endif /* PPE_STRING_H */
