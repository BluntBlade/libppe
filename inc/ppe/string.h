#ifndef PPE_STRING_H
#define PPE_STRING_H 1 

#include <stdarg.h>

#include "ppe/types.h"
#include "ppe/macros.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ==== Declaration ==== */

/* ---- Types ---- */

struct ppe_string_st; 
typedef struct ppe_string_st * ppe_string;

typedef struct _PPE_CSTR
{
    const char * str;
    ppe_size len;
} ppe_cstr_st;

/* ---- Macros ---- */

#define PPE_STR_AUTO_LENGTH (~((ppe_size)0L))
#define PPE_STR_LIST_END NULL

/* ---- Interfaces ---- */

/* ---- Functions ---- */

/* -- Create & Destroy -- */

PPE_API extern ppe_string ppe_cs_clone(const ppe_char * restrict str, ppe_size len);
PPE_API extern ppe_string ppe_cs_join_var(const char * restrict deli, const char * restrict str1, ppe_size len1, const char * restrict str2, ppe_size len2, va_list args);
PPE_API extern ppe_string ppe_cs_join_list(const char * restrict deli, ppe_cstr_st * restrict strs, ppe_uint n);

PPE_API extern ppe_size ppe_cs_snprintf(char * restrict buf, ppe_size * restrict buf_size, const char * restrict format, ...);

PPE_API extern ppe_string ppe_str_clone(const ppe_string restrict str);
PPE_API extern ppe_string ppe_str_join_var(const char * restrict deli, const ppe_string restrict str1, const ppe_string restrict str2, va_list args);
PPE_API extern ppe_string ppe_str_join_list(const char * restrict deli, const ppe_string * restrict strs, ppe_uint n);

PPE_API extern ppe_string ppe_str_sprintf(const char * restrict format, ...);

PPE_API extern void ppe_str_destroy(ppe_string restrict str);

/* -- Property -- */

PPE_API extern const char * ppe_str_cstr(ppe_string restrict str);
PPE_API extern ppe_size ppe_str_size(ppe_string restrict str);

/* -- Wrapper -- */

static inline ppe_string ppe_cs_join(const char * restrict deli, const char * restrict str1, ppe_size len1, const char * restrict str2, ppe_size len2, ...)
{
    ppe_string str = NULL;
    va_list args;

    va_start(len2, args);
    str = ppe_cs_join_var(deli, str1, len1, str2, len2, args);
    va_end(args);
    return str;
}

static inline ppe_string ppe_cs_concat(const char * restrict str1, ppe_size len1, const char * restrict str2, ppe_size len2, ...)
{
    ppe_string str = NULL;
    va_list args;

    va_start(len2, args);
    str = ppe_cs_join_var("", str1, len1, str2, len2, args);
    va_end(args);
    return str;
}

static inline ppe_string ppe_cs_concat_list(const char * restrict deli, ppe_cstr_st * restrict strs, ppe_uint n)
{
    return ppe_cs_join_list("", strs, n);
}

static inline ppe_string ppe_str_join(const char * restrict deli, const ppe_string restrict str1, const ppe_string restrict str2, ...)
{
    ppe_string str = NULL;
    va_list args;

    va_start(len2, args);
    str = ppe_str_join_var(deli, str1, len1, str2, len2, args);
    va_end(args);
    return str;
}

static inline ppe_string ppe_str_concat(const ppe_string restrict str1, const ppe_string restrict str2, ...)
{
    ppe_string str = NULL;
    va_list args;

    va_start(len2, args);
    str = ppe_str_join_var("", str1, len1, str2, len2, args);
    va_end(args);
    return str;
}

static inline ppe_string ppe_str_concat_list(const char * restrict deli, const ppe_string * restrict strs, ppe_uint n)
{
    return ppe_str_join_list("", strs, n);
}

#ifdef __cplusplus
}
#endif

#endif /* PPE_STRING_H */
