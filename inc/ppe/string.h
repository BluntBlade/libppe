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

#define PPE_STR_DETECT_SIZE ((ppe_ssize)-1L)
#define PPE_STR_MAX_SIZE (1L << (sizeof(ppe_ssize) * 8 - 1))

#define PPE_STR_ARG_END NULL

/* ---- Interfaces ---- */

/* ---- Functions ---- */

/* -- Preset values -- */

PPE_API extern const ppe_string ppe_str_get_empty(void);

/* -- Create & Destroy -- */

/* _cs_ series */

PPE_API extern ppe_string ppe_cs_clone(const ppe_char * restrict s, const ppe_ssize sz);

/* join */

PPE_API extern ppe_string ppe_cs_join_cstr(const char * restrict d, const ppe_ssize dsz, const char * restrict s1, const ppe_ssize sz1, const char * restrict s2, const ppe_ssize sz2, ...);
PPE_API extern ppe_string ppe_cs_join(const char * restrict d, const ppe_ssize dsz, const ppe_string restrict s1, const ppe_string restrict s2, ...);

PPE_API extern ppe_string ppe_cs_join_cstr_2(const char * restrict d, const ppe_ssize dsz, const ppe_ssize dsz, const char * restrict s1, const ppe_ssize sz1, const char * restrict s2, const ppe_ssize sz2);
PPE_API extern ppe_string ppe_cs_join_2(const char * restrict d, const ppe_ssize dsz, const ppe_ssize dsz, const ppe_string restrict s1, const ppe_string restrict s2);

/* concat */

PPE_API extern ppe_string ppe_cs_concat_cstr(const char * restrict s1, const ppe_ssize sz1, const char * restrict s2, const ppe_ssize sz2, ...);
PPE_API extern ppe_string ppe_cs_concat_cstr_2(const char * restrict s1, const ppe_ssize sz1, const char * restrict s2, const ppe_ssize sz2);

/* split */

PPE_API extern ppe_str_bunch ppe_cs_split_cstr(const char * restrict d, const ppe_ssize dsz, const char * restrict s, const ppe_ssize sz, const ppe_int n);
PPE_API extern ppe_str_bunch ppe_cs_split(const char * restrict d, const ppe_ssize dsz, const ppe_string restrict s, const ppe_int n);

/* _str_ series */

PPE_API extern ppe_string ppe_str_clone(const ppe_string restrict s);

/* join */

PPE_API extern ppe_string ppe_str_join_cstr(const ppe_string restrict d, const char * restrict s1, const ppe_ssize sz1, const char * restrict s2, const ppe_ssize sz2, ...);
PPE_API extern ppe_string ppe_str_join(const ppe_string restrict d, const ppe_string restrict s1, const ppe_string restrict s2, ...);

PPE_API extern ppe_string ppe_str_join_cstr_2(const char * restrict d, const char * restrict s1, const ppe_ssize sz1, const char * restrict s2, const ppe_ssize sz2);
PPE_API extern ppe_string ppe_str_join_2(const ppe_string restrict d, const ppe_string restrict s1, const ppe_string restrict s2);

/* concat */

PPE_API extern ppe_string ppe_str_concat(const ppe_string restrict s1, const ppe_string restrict s2, ...);
PPE_API extern ppe_string ppe_str_concat_2(const ppe_string restrict s1, const ppe_string restrict s2);

/* split */

PPE_API extern ppe_str_bunch ppe_str_split_cstr(const ppe_string restrict d, const char * restrict s, const ppe_ssize sz, const ppe_int n);
PPE_API extern ppe_str_bunch ppe_str_split(const ppe_string restrict d, const ppe_string restrict s, const ppe_int n);

/* destroy */

PPE_API extern void ppe_str_destroy(ppe_string restrict s);

/* -- Formatting -- */

/* PPE_API extern ppe_ssize ppe_misc_snprintf(char * restrict buf, ppe_ssize * restrict buf_size, const char * restrict format, ...); */
PPE_API extern ppe_string ppe_cs_vsprintf(const char * restrict fmt, va_list args);

/* -- Property -- */

PPE_API extern const char * ppe_str_cstr(ppe_string restrict s);
PPE_API extern ppe_ssize ppe_str_size(ppe_string restrict s);

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

PPE_API extern ppe_bool ppe_sbc_push_refer_to_cstr(ppe_str_bunch restrict bc, const char * restrict s, const ppe_ssize sz);
PPE_API extern ppe_bool ppe_sbc_push_refer_to(ppe_str_bunch restrict bc, const ppe_string restrict s);

PPE_API extern ppe_bool ppe_sbc_push_copy_of_cstr(ppe_str_bunch restrict bc, const char * restrict s, const ppe_ssize sz);
PPE_API extern ppe_bool ppe_sbc_push_copy_of(ppe_str_bunch restrict bc, const ppe_string restrict s);

PPE_API extern void ppe_sbc_pop(ppe_str_bunch restrict bc);

PPE_API extern ppe_string ppe_sbc_join_by_cstr(const ppe_str_bunch restrict bc, const char * restrict s, const ppe_ssize sz);
PPE_API extern ppe_string ppe_sbc_join_by(const ppe_str_bunch restrict bc, const ppe_string restrict s);

PPE_API extern ppe_string ppe_sbc_concat(const ppe_str_bunch restrict bc);

PPE_API extern ppe_bool ppe_sbc_next_buffer(const ppe_str_bunch restrict bc, const char ** restrict buf, ppe_size * restrict sz, void ** restrict dat);

PPE_API extern ppe_bool ppe_sbc_reference(const ppe_str_bunch restrict bc, const ppe_uint idx, const char ** restrict s, ppe_ssize * restrict sz);
PPE_API extern ppe_uint ppe_sbc_string_count(const ppe_str_bunch restrict bc);
PPE_API extern ppe_ssize ppe_sbc_total_size(const ppe_str_bunch restrict bc);

/* ==== Declaration : String Finder ==== */

/* ---- Types ---- */

struct ppe_str_finder_st;
typedef struct ppe_str_finder_st * ppe_str_finder;

/* ---- Functions ---- */

PPE_API extern ppe_str_finder ppe_sfd_create_for_cstr(const char * restrict s, const ppe_ssize sz);
PPE_API extern ppe_str_finder ppe_sfd_create_for(const ppe_string restrict s);
PPE_API extern void ppe_sfd_destroy(ppe_str_finder restrict fd);

PPE_API extern ppe_bool ppe_sfd_reset_for_cstr(ppe_str_finder restrict fd, const char * restrict s, const ppe_ssize sz);
PPE_API extern ppe_bool ppe_sfd_reset_for(ppe_str_finder restrict fd, const ppe_string restrict s);

PPE_API extern ppe_int ppe_sfd_find_n_for_cstr(ppe_str_finder restrict fd, const char * s restrict, const ppe_ssize sz, const ppe_int n, const ppe_bool copy, ppe_str_bunch restrict bcc, ppe_str_bunch restrict bcs);
PPE_API extern ppe_int ppe_sfd_find_n_for(ppe_str_finder restrict fd, const ppe_string restrict s, const ppe_int n, const ppe_bool copy, pe_str_bunch restrict bcc, ppe_str_bunch restrict bcs);

PPE_API extern ppe_bool ppe_sfd_find_last_component(ppe_str_finder restrict fd, const ppe_bool copy, ppe_str_bunch restrict bcc);

/* ==== Declarations : String ==== */

/* ---- Functions Defered to Implement ---- */

/* -- cs module -- */

/* -- str module -- */

#ifdef __cplusplus
}
#endif

#endif /* PPE_STRING_H */
