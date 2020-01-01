#ifndef PPE_STRING_H
#define PPE_STRING_H 1 

#include <stdarg.h>

#include "ppe/types.h"
#include "ppe/macros.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ==== Declaration : String ================================================ */

/* ---- Types --------------------------------------------------------------- */

struct ppe_str_bunch_st;

typedef char * ppe_cstr;

struct ppe_string_st; 
typedef struct ppe_string_st * ppe_string;

/* ---- Macros -------------------------------------------------------------- */

#define PPE_STR_DETECT_SIZE ((ppe_ssize)-1L)
#define PPE_STR_MAX_SIZE (1L << (sizeof(ppe_ssize) * 8 - 1))

#define PPE_STR_ARG_END NULL

/* ==== Declaration : C-String ============================================== */

/* ---- Functions ----------------------------------------------------------- */

/* -- Preset values -- */

PPE_API extern const ppe_cstr ppe_cs_empty(void);

/* -- Property -- */

PPE_API extern ppe_cs_size(const ppe_cstr restrict s);

/* -- Test -- */

PPE_API extern ppe_bool ppe_cs_is_empty(const ppe_cstr restrict s);
PPE_API extern ppe_int ppe_cs_compare(const ppe_cstr restrict s1, const ppe_cstr restrict s2);

/* -- Find -- */

PPE_API extern const ppe_cstr ppe_cs_find(const ppe_cstr restrict s, const ppe_cstr restrict t);

/* -- Create & Destroy -- */

PPE_API extern void ppe_cs_destroy(ppe_cstr restrict s);
PPE_API extern ppe_cstr ppe_cs_create(const ppe_cstr restrict s, const ppe_size sz);

/* -- Join & Concat -- */

PPE_API extern ppe_cstr ppe_cs_join(const ppe_cstr restrict d, const ppe_cstr restrict s1, const ppe_cstr restrict s2, ...);
PPE_API extern ppe_cstr ppe_cs_concat(const ppe_cstr restrict d, const ppe_cstr restrict s1, const ppe_cstr restrict s2, ...);

/* -- Wrapper -- */

static inline ppe_bool ppe_cs_equal_to(const ppe_cstr restrict s1, const ppe_cstr restrict s2)
{
    return ppe_cs_compare(s1, s2) == 0;
}

static inline ppe_bool ppe_cs_is_less_than(const ppe_cstr restrict s1, const ppe_cstr restrict s2)
{
    return ppe_cs_compare(s1, s2) < 0;
}

static inline ppe_bool ppe_cs_is_less_than_or_equal_to(const ppe_cstr restrict s1, const ppe_cstr restrict s2)
{
    return ppe_cs_compare(s1, s2) <= 0;
}

static inline ppe_bool ppe_cs_is_greater_than(const ppe_cstr restrict s1, const ppe_cstr restrict s2)
{
    return ppe_cs_compare(s1, s2) > 0;
}

static inline ppe_bool ppe_cs_is_greater_than_or_equal_to(const ppe_cstr restrict s1, const ppe_cstr restrict s2)
{
    return ppe_cs_compare(s1, s2) >= 0;
}

static inline ppe_cstr ppe_cs_clone(const ppe_cstr restrict s)
{
    return ppe_cs_create(s, ppe_cs_size(s));
}

/* ==== Declaration : String ================================================ */

/* ---- Functions ----------------------------------------------------------- */

/* -- Preset values -- */

PPE_API extern const ppe_string ppe_str_empty(void);

/* -- Property -- */

PPE_API extern const ppe_char * pp_str_addr(ppe_string restrict s);
PPE_API extern ppe_ssize ppe_str_size(ppe_string restrict s);

/* -- Comparison -- */

PPE_API extern ppe_bool ppe_str_equals_to(const ppe_string restrict s1, const ppe_string restrict s2);
PPE_API extern ppe_bool ppe_str_less_than(const ppe_string restrict s1, const ppe_string restrict s2);
PPE_API extern ppe_bool ppe_str_greater_than(const ppe_string restrict s1, const ppe_string restrict s2);

static inline ppe_bool ppe_str_less_than_or_equals_to(const ppe_string restrict s1, const ppe_string restrict s2)
{
    return ! ppe_str_greater_than(s1, s2);
}

static inline ppe_bool ppe_str_greater_than_or_equals_to(const ppe_string restrict s1, const ppe_string restrict s2)
{
    return ! ppe_str_less_than(s1, s2);
}

/* -- Create & Destroy -- */

PPE_API extern void ppe_str_destroy(ppe_string restrict s);

/* _cs_ series */

/* split */

PPE_API extern struct ppe_str_bunch_st * ppe_cs_split(const ppe_char * restrict d, const ppe_ssize dsz, const ppe_char * restrict s, const ppe_ssize sz, const ppe_int n);
PPE_API extern struct ppe_str_bunch_st * ppe_cs_split_str(const ppe_char * restrict d, const ppe_ssize dsz, const ppe_string restrict s, const ppe_int n);

/* _str_ series */

/* join */

PPE_API extern ppe_string ppe_str_join_cstr(const ppe_string restrict d, const ppe_char * restrict s1, const ppe_ssize sz1, const ppe_char * restrict s2, const ppe_ssize sz2, ...);
PPE_API extern ppe_string ppe_str_join(const ppe_string restrict d, const ppe_string restrict s1, const ppe_string restrict s2, ...);

static inline ppe_string ppe_str_join_cstr_2(const ppe_char * restrict d, const ppe_char * restrict s1, const ppe_ssize sz1, const ppe_char * restrict s2, const ppe_ssize sz2)
{
    return ppe_str_join_cstr(d, s1, sz1, s2, sz2, PPE_STR_ARG_END);
}

static inline ppe_string ppe_str_join_2(const ppe_string restrict d, const ppe_string restrict s1, const ppe_string restrict s2)
{
    return ppe_str_join(d, s1, s2, PPE_STR_ARG_END);
}

/* concat */

PPE_API extern ppe_string ppe_str_concat(const ppe_string restrict s1, const ppe_string restrict s2, ...);

static inline ppe_string ppe_str_concat_2(const ppe_string restrict s1, const ppe_string restrict s2)
{
    return ppe_str_join(ppe_str_empty(), s1, s2, PPE_STR_ARG_END);
}

/* split */

PPE_API extern struct ppe_str_bunch_st * ppe_str_split_cstr(const ppe_string restrict d, const ppe_char * restrict s, const ppe_ssize sz, const ppe_int n);

/* -- Formatting -- */

/* PPE_API extern ppe_ssize ppe_misc_snprintf(ppe_char * restrict buf, ppe_ssize * restrict buf_size, const ppe_char * restrict format, ...); */
PPE_API extern ppe_string ppe_cs_vsprintf(const ppe_char * restrict fmt, va_list args);

/* -- Wrapper -- */

/* _cs_ series */

static inline ppe_string ppe_cs_join_2(const ppe_char * restrict d, const ppe_ssize dsz, const ppe_ssize dsz, const ppe_char * restrict s1, const ppe_ssize sz1, const ppe_char * restrict s2, const ppe_ssize sz2)
{
    return ppe_cs_join(d, dsz, s1, sz1, s2, sz2, PPE_STR_ARG_END);
}

static inline ppe_string ppe_cs_concat_2(const ppe_char * restrict s1, const ppe_ssize sz1, const ppe_char * restrict s2, const ppe_ssize sz2)
{
    return ppe_cs_join("", 0, s1, sz1, s2, sz2, PPE_STR_ARG_END);
}

static ppe_string ppe_cs_sprintf(const ppe_char * restrict fmt, ...)
{
    ppe_string new_str = NULL;
    va_list args;

    va_start(args, fmt);
    new_str = ppe_cs_vsprintf(fmt, args);
    va_end(args);
    return new_str;
}

/* _str_ series */

static inline ppe_str_clone(const ppe_string restrict s)
{
    assert(s != NULL);
    return ppe_cs_clone(ppe_str_addr(s), ppe_str_size(s));
}

static inline struct ppe_str_bunch_st * ppe_str_split(const ppe_string restrict d, const ppe_string restrict s, const ppe_int n)
{
    return ppe_str_split_cstr(d, ppe_str_addr(s), ppe_str_size(s), n);
}

#ifdef __cplusplus
}
#endif

#endif /* PPE_STRING_H */

