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

/* ---- Macros ---- */

#define PPE_STR_AUTO_LENGTH ((ppe_size)0L)
#define PPE_STR_INVALID_LENGTH (~ (ppe_size)0L)
#define PPE_STR_LIST_END (~ (void*)0)

/* ---- Interfaces ---- */

/* ---- Functions ---- */

/* -- Create & Destroy -- */

PPE_API extern ppe_string ppe_cs_clone(const ppe_char * restrict src, const ppe_size len);
PPE_API extern ppe_string ppe_cs_join_var(const char * restrict deli, const char * restrict src1, const ppe_size len1, const char * restrict src2, const ppe_size len2, va_list args);
PPE_API extern ppe_string ppe_cs_join_list(const char * restrict deli, const char ** restrict srcs, const ppe_uint n);
PPE_API extern ppe_string ppe_cs_join_2(const char * restrict deli, const char * restrict src1, const ppe_size len1, const char * restrict src2, const ppe_size len2);

/* PPE_API extern ppe_size ppe_misc_snprintf(char * restrict buf, ppe_size * restrict buf_size, const char * restrict format, ...); */
PPE_API extern ppe_string ppe_cs_vsprintf(const char * restrict fmt, va_list args);

PPE_API extern ppe_string ppe_str_clone(const ppe_string restrict src);
PPE_API extern ppe_string ppe_str_join_var(const char * restrict deli, const ppe_string restrict src1, const ppe_string restrict src2, va_list args);
PPE_API extern ppe_string ppe_str_join_list(const char * restrict deli, const ppe_string * restrict srcs, const ppe_uint n);
PPE_API extern ppe_string ppe_str_join_2(const char * restrict deli, const ppe_string restrict src1, const ppe_string restrict src2);

PPE_API extern void ppe_str_destroy(ppe_string restrict src);

PPE_API extern const ppe_string ppe_str_get_empty(void);

/* -- Property -- */

PPE_API extern const char * ppe_str_cstr(ppe_string restrict str);
PPE_API extern ppe_size ppe_str_size(ppe_string restrict str);

/* -- Wrapper -- */

static inline ppe_string ppe_cs_join(const char * restrict deli, const char * restrict src1, const ppe_size len1, const char * restrict src2, const ppe_size len2, ...)
{
    ppe_string new_str = NULL;
    va_list args;

    va_start(len2, args);
    new_str = ppe_cs_join_var(deli, src1, len1, src2, len2, args);
    va_end(args);
    return new_str;
}

static inline ppe_string ppe_cs_concat_2(const char * restrict src1, const ppe_size len1, const char * restrict src2, const ppe_size len2)
{
    return ppe_cs_join_2("", src1, len1, src2, len2);
}

static inline ppe_string ppe_cs_concat(const char * restrict src1, const ppe_size len1, const char * restrict src2, const ppe_size len2, ...)
{
    ppe_string new_str = NULL;
    va_list args;

    va_start(len2, args);
    new_str = ppe_cs_join_var("", src1, len1, src2, len2, args);
    va_end(args);
    return new_str;
}

static inline ppe_string ppe_cs_concat_list(const char * restrict deli, ppe_text_st * restrict srcs, const ppe_uint n)
{
    return ppe_cs_join_list("", srcs, n);
}

static ppe_string ppe_cs_sprintf(const char * restrict fmt, ...)
{
    ppe_string new_str = NULL;
    va_list args;

    va_start(fmt, args);
    new_str = ppe_cs_vsprintf(fmt, args);
    va_end(args);
    return new_str;
}

static inline ppe_string ppe_str_join(const char * restrict deli, const ppe_string restrict src1, const ppe_string restrict src2)
{
    return ppe_str_join_2("", src1, src2);
}

static inline ppe_string ppe_str_join(const char * restrict deli, const ppe_string restrict src1, const ppe_string restrict src2, ...)
{
    ppe_string new_str = NULL;
    va_list args;

    va_start(src2, args);
    new_str = ppe_str_join_var(deli, src1, src2, args);
    va_end(args);
    return new_str;
}

static inline ppe_string ppe_str_concat(const ppe_string restrict src1, const ppe_string restrict src2, ...)
{
    ppe_string new_str = NULL;
    va_list args;

    va_start(src2, args);
    new_str = ppe_str_join_var("", src1, src2, args);
    va_end(args);
    return new_str;
}

static inline ppe_string ppe_str_concat_list(const char * restrict deli, const ppe_string * restrict srcs, const ppe_uint n)
{
    return ppe_str_join_list("", srcs, n);
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
