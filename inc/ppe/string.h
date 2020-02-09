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

struct ppe_cs_snippet_st;
typedef struct ppe_cs_snippet_st * ppe_cs_snippet;

struct ppe_string_st; 
typedef struct ppe_string_st * ppe_string;

typedef enum
{
    PPE_PTR_JOIN_END = 0,
    PPE_PTR_JOIN_UNSET_DELIMITER = 1,
    PPE_PTR_JOIN_SET_DELIMITER_CSTR = 2,
    PPE_PTR_JOIN_SET_DELIMITER_CSTR_WITH_SIZE = 3,
    PPE_PTR_JOIN_SET_DELIMITER_STRING = 4,
    PPE_PTR_JOIN_ADD_ITEM_CSTR = 5,
    PPE_PTR_JOIN_ADD_ITEM_CSTR_WITH_SIZE = 6,
    PPE_PTR_JOIN_ADD_ITEM_STRING = 7,
    PPE_PTR_JOIN_ADD_ITEM_SNIPPET = 8
} ppe_str_join_action;

typedef enum
{
    PPE_STR_OPT_DIRECT_LEFT = 0x00000001,
    PPE_STR_OPT_DIRECT_RIGHT = 0x00000002,
    PPE_STR_OPT_DIRECT_BOTH = 0x00000003,
    PPE_STR_OPT_MEASURE_SIZE = 0x20000000,
    PPE_STR_OPT_DONT_TRUNCATE = 0x40000000,
    PPE_STR_OPT_NEW_STRING = 0x80000000
} ppe_str_option;

/* ---- Macros -------------------------------------------------------------- */

#define PPE_STR_DETECT_SIZE ((ppe_ssize)-1L)
#define PPE_STR_MAX_SIZE (1L << (sizeof(ppe_size) * 8 - 1))

#define PPE_STR_SPACES " \t"

#define PPE_STR_WINDOWS_NEWLINE "\r\n"
#define PPE_STR_UNIX_NEWLINE "\n"

#if defined(PPE_CFG_OS_WINDOWS)
#define PPE_STR_NEWLINE PPE_STR_WINDOWS_NEWLINE
#else
#define PPE_STR_NEWLINE PPE_STR_UNIX_NEWLINE
#endif

#define PPE_STR_ARG_END NULL

/* ==== Declaration : C-String Snippet ====================================== */

/* -- Property -- */

PPE_API extern ppe_uint ppe_cspt_count(const ppe_cs_snippet restrict spt);
PPE_API extern ppe_uint ppe_cspt_capacity(const ppe_cs_snippet restrict spt);

/* -- Create & Destroy -- */

PPE_API extern ppe_cs_snippet ppe_cspt_create(const ppe_uint cap);
PPE_API extern void ppe_cspt_destroy(ppe_cs_snippet restrict spt);
PPE_API extern void ppe_cspt_reset(ppe_cs_snippet restrict spt);

/* -- Manipulators -- */

PPE_API extern ppe_bool ppe_cspt_get(ppe_cs_snippet restrict spt, const ppe_uint idx, const ppe_cstr * restrict s, ppe_size * restrict sz);
PPE_API extern ppe_bool ppe_cspt_append(ppe_cs_snippet restrict spt, const ppe_cstr restrict s, const ppe_size sz);

/* -- Producer -- */

PPE_API extern ppe_cstr ppe_cspt_clone(ppe_cs_snippet restrict spt, const ppe_uint idx, ppe_cstr restrict b, ppe_size * bsz);
PPE_API extern ppe_cstr * ppe_cspt_clone_some(const ppe_cs_snippet restrict spt, const ppe_uint idx, ppe_uint * restrict n);

/* -- Wrappers -- */

static inline ppe_bool ppe_cspt_is_empty(const ppe_cs_snippet restrict spt)
{
    return ppe_cspt_count(spt) == 0;
}

static inline ppe_bool ppe_cspt_is_full(const ppe_cs_snippet restrict spt)
{
    return ppe_cspt_count(spt) == ppe_cspt_capacity(spt);
}

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

/* -- Substring -- */

PPE_API extern ppe_cstr ppe_cs_substr(const ppe_cstr restrict s, const ppe_size off, const ppe_size ssz, ppe_cstr restrict b, ppe_size * bsz, ppe_str_option opt);

/* -- Trim & Chomp -- */

PPE_API extern ppe_cstr ppe_cs_trim_bytes(const ppe_cstr restrict s, const ppe_cstr restrict t, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt);
PPE_API extern ppe_cstr ppe_cs_chop(const ppe_cstr restrict s, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt);
PPE_API extern ppe_cstr ppe_cs_chomp(const ppe_cstr restrict s, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt);

/* -- Join & Concat -- */

PPE_API extern ppe_cstr ppe_cs_join(const ppe_cstr restrict d, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt, ...);
PPE_API extern ppe_cstr ppe_cs_concat(ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt, ...);

/* -- Split -- */

PPE_API extern ppe_cs_snippet ppe_cs_split(const ppe_cstr restrict s, const ppe_cstr restrict d, ppe_cs_snippet restrict spt, ppe_uint * restrict n, const ppe_cstr * restrict ss, const ppe_str_option opt);

/* -- Replace & Substitute -- */

PPE_API extern ppe_cs_cstr ppe_cs_replace(const ppe_cstr restrict s, const ppe_size off, const ppe_size ssz, const ppe_cstr restrict to, ppe_cs_cstr restrict b, ppe_size * restrict bsz, const ppe_str_option opt);
PPE_API extern ppe_cs_cstr ppe_cs_substitute(const ppe_cstr restrict s, const ppe_cstr restrict from, const ppe_cstr restrict to, ppe_cs_cstr restrict b, ppe_size * restrict bsz, ppe_uint * restrict n, const ppe_str_option opt);

/* -- Format -- */

PPE_API extern ppe_cstr ppe_cs_sprintf(ppe_cstr restrict b, ppe_size * restrict bsz, const ppe_str_option opt, const ppe_cstr restrict fmt, ...);

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

static inline ppe_cstr ppe_cs_trim_left(const ppe_cstr restrict s, const ppe_cstr restrict t, ppe_cstr * restring b, ppe_size * bsz)
{
    return ppe_cs_trim_bytes(s, PPE_STR_SPACES, b, bsz, PPE_STR_OPT_DIRECT_LEFT);
}

static inline ppe_cstr ppe_cs_trim_right(const ppe_cstr restrict s, const ppe_cstr restrict t, ppe_cstr * restring b, ppe_size * bsz)
{
    return ppe_cs_trim_bytes(s, PPE_STR_SPACES, b, bsz, PPE_STR_OPT_DIRECT_RIGHT);
}

static inline ppe_cstr ppe_cs_trim(const ppe_cstr restrict s, ppe_cstr * restring b, ppe_size * bsz)
{
    return ppe_cs_trim_bytes(s, PPE_STR_SPACES, b, bsz, PPE_STR_OPT_DIRECT_BOTH);
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

/* -- Join & Concat -- */

PPE_API extern ppe_string ppe_str_join(const ppe_string restrict d, const ppe_str_option opt, ...);

/* TODO: ppe_str_concat() as a macro with variable arguments. Need C99 support. */

/* -- Split -- */

PPE_API extern ppe_cs_snippet ppe_str_split(const ppe_string restrict d, const ppe_string restrict s, const const ppe_uint * restrict n, const ppe_str_option opt);

/* -- Wrapper -- */

static inline ppe_string ppe_str_join_two(const ppe_string restrict d, const ppe_string restrict s1, const ppe_string restrict s2)
{
    return ppe_str_join(d, PPE_PTR_JOIN_ADD_ITEM_STRING, s1, PPE_PTR_JOIN_ADD_ITEM_STRING, s2, PPE_PTR_JOIN_END);
}

static inline ppe_string ppe_str_concat_two(const ppe_string restrict s1, const ppe_string restrict s2)
{
    return ppe_str_join(ppe_str_empty(), PPE_PTR_JOIN_ADD_ITEM_STRING, s1, PPE_PTR_JOIN_ADD_ITEM_STRING, s2, PPE_PTR_JOIN_END);
}

#ifdef __cplusplus
}
#endif

#endif /* PPE_STRING_H */

