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
    ppe_size sz;
    const char * buf;
} ppe_text_st, *ppe_text;

/* ---- Macros ---- */

#define PPE_STR_DETECT_LENGTH (~ (ppe_size)0L)
#define PPE_STR_INVALID_LENGTH (~ (ppe_size)0L)
#define PPE_STR_MAX_LENGTH ((~ (ppe_size)0L) / 2)

#define PPE_STR_ARG_END NULL

#define PPE_STR_MAX_INPUT_N 32

/* ---- Interfaces ---- */

/* ---- Functions ---- */

/* -- Preset values -- */

PPE_API extern const ppe_string ppe_str_get_empty(void);

/* -- Create & Destroy -- */

/* _cs_ series */

PPE_API extern ppe_string ppe_cs_clone(const ppe_char * restrict s, const ppe_size sz);

/* join */

PPE_API extern ppe_string ppe_cs_join_cstr(const char * restrict d, const ppe_size dsz, const char * restrict s1, const ppe_size sz1, const char * restrict s2, const ppe_size sz2, ...);
PPE_API extern ppe_string ppe_cs_join(const char * restrict d, const ppe_size dsz, const ppe_string restrict s1, const ppe_string restrict s2, ...);

PPE_API extern ppe_string ppe_cs_join_cstr_2(const char * restrict d, const ppe_size dsz, const ppe_size dsz, const char * restrict s1, const ppe_size sz1, const char * restrict s2, const ppe_size sz2);
PPE_API extern ppe_string ppe_cs_join_2(const char * restrict d, const ppe_size dsz, const ppe_size dsz, const ppe_string restrict s1, const ppe_string restrict s2);

/* concat */

PPE_API extern ppe_string ppe_cs_concat_cstr(const char * restrict s1, const ppe_size sz1, const char * restrict s2, const ppe_size sz2, ...);
PPE_API extern ppe_string ppe_cs_concat_cstr_2(const char * restrict s1, const ppe_size sz1, const char * restrict s2, const ppe_size sz2);

/* split */

PPE_API extern ppe_string * ppe_cs_split_cstr(const char * restrict d, const ppe_size dsz, const char * restrict s, const ppe_size sz, ppe_uint * restrict n);
PPE_API extern ppe_string * ppe_cs_split(const char * restrict d, const ppe_size dsz, const ppe_string restrict s, ppe_uint * restrict n);

/* _str_ series */

PPE_API extern ppe_string ppe_str_clone(const ppe_string restrict s);

/* join */

PPE_API extern ppe_string ppe_str_join_cstr(const ppe_string restrict d, const char * restrict s1, const ppe_size sz1, const char * restrict s2, const ppe_size sz2, ...);
PPE_API extern ppe_string ppe_str_join(const ppe_string restrict d, const ppe_string restrict s1, const ppe_string restrict s2, ...);

PPE_API extern ppe_string ppe_str_join_cstr_2(const char * restrict d, const char * restrict s1, const ppe_size sz1, const char * restrict s2, const ppe_size sz2);
PPE_API extern ppe_string ppe_str_join_2(const ppe_string restrict d, const ppe_string restrict s1, const ppe_string restrict s2);

/* concat */

PPE_API extern ppe_string ppe_str_concat(const ppe_string restrict s1, const ppe_string restrict s2, ...);
PPE_API extern ppe_string ppe_str_concat_2(const ppe_string restrict s1, const ppe_string restrict s2);

/* split */

PPE_API extern ppe_string * ppe_str_split_cstr(const ppe_string restrict d, const char * restrict s, const ppe_size sz, ppe_uint * restrict n);
PPE_API extern ppe_string * ppe_str_split(const ppe_string restrict d, const ppe_string restrict s, ppe_uint * restrict n);

/* destroy */

PPE_API extern void ppe_str_destroy(ppe_string restrict s);

/* -- Formatting -- */

/* PPE_API extern ppe_size ppe_misc_snprintf(char * restrict buf, ppe_size * restrict buf_size, const char * restrict format, ...); */
PPE_API extern ppe_string ppe_cs_vsprintf(const char * restrict fmt, va_list args);

/* -- Property -- */

PPE_API extern const char * ppe_str_cstr(ppe_string restrict s);
PPE_API extern ppe_size ppe_str_size(ppe_string restrict s);

/* -- Comparison -- */

PPE_API extern ppe_bool ppe_str_equals(const ppe_string restrict s1, const ppe_string restrict s2);
PPE_API extern ppe_bool ppe_str_less_than(const ppe_string restrict s1, const ppe_string restrict s2);
PPE_API extern ppe_bool ppe_str_greater_than(const ppe_string restrict s1, const ppe_string restrict s2);

/* -- Wrapper -- */

/* _cs_ series */

static ppe_string ppe_cs_sprintf(const char * restrict fmt, ...)
{
    ppe_string new_str = NULL;
    va_list args;

    va_start(args, fmt);
    new_str = ppe_cs_vsprintf(fmt, args);
    va_end(args);
    return new_str;
}

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

/* ==== Declaration : String Finder ==== */

/* ---- Types ---- */

struct ppe_str_finder_st;
typedef struct ppe_str_finder_st * ppe_str_finder;

/* ---- Functions ---- */

PPE_API extern ppe_str_finder ppe_sfd_create_for_cstr(const char * restrict s, const ppe_size sz);
PPE_API extern ppe_str_finder ppe_sfd_create_for(const ppe_string restrict s);
PPE_API extern void ppe_sfd_destroy(ppe_str_finder restrict fd);

PPE_API extern ppe_bool ppe_sfd_reset_for_cstr(ppe_str_finder restrict fd, const char * restrict s, const ppe_size sz);
PPE_API extern ppe_bool ppe_sfd_reset_for(ppe_str_finder restrict fd, const ppe_string restrict s);

PPE_API extern ppe_bool ppe_sfd_find_component_by_cstr(ppe_str_finder restrict fd, const char * s restrict, const ppe_size sz, ppe_uint * restrict n, ppe_str_bunch restrict bc);
PPE_API extern ppe_bool ppe_sfd_find_component_by(ppe_str_finder restrict fd, const ppe_string restrict s, ppe_uint * restrict n, ppe_str_bunch restrict bc);

PPE_API extern ppe_bool ppe_sfd_refer_last_component(ppe_str_finder restrict fd, ppe_str_bunch restrict bc);

PPE_API extern ppe_bool ppe_sfd_find_substring_by_cstr(ppe_str_finder restrict fd, const char * s restrict, const ppe_size sz, ppe_uint * restrict n, ppe_str_bunch restrict bc);
PPE_API extern ppe_bool ppe_sfd_find_substring_by(ppe_str_finder restrict fd, const ppe_string restrict s, ppe_uint * restrict n, ppe_str_bunch restrict bc);

/* ==== Declarations : String ==== */

/* ---- Functions Defered to Implement ---- */

/* -- cs module -- */

/* -- str module -- */

#ifdef __cplusplus
}
#endif

#endif /* PPE_STRING_H */
