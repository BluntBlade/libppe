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

typedef ppe_char * ppe_cstr;
typedef const ppe_char * ppe_cstr_c;

struct PPE_CS_SNIPPET;
typedef struct PPE_CS_SNIPPET * ppe_cs_snippet;

struct PPE_STRING; 
typedef struct PPE_STRING * ppe_string;

typedef enum
{
    PPE_CS_JOIN_END = 0,
    PPE_CS_JOIN_CSTR = 1,
    PPE_CS_JOIN_CSTR_WITH_SIZE = 2,
    PPE_CS_JOIN_STRING = 3,
    PPE_CS_JOIN_SNIPPET = 4,
    PPE_CS_JOIN_SET_DELIMITER= 0x80000000
} ppe_str_join_action;

enum
{
    PPE_STR_OPT_NONE = 0x00000000,
    PPE_STR_OPT_LEFT_END = 0x00000001,
    PPE_STR_OPT_RIGHT_END = 0x00000002,
    PPE_STR_OPT_BOTH_ENDS = 0x00000003,
    PPE_STR_OPT_CSTR_ARG = 0x01000000,
    PPE_STR_OPT_MEASURE_SIZE = 0x20000000,
    PPE_STR_OPT_DONT_TRUNCATE = 0x40000000,
    PPE_STR_OPT_NEW_STRING = 0x80000000
};

typedef ppe_uint ppe_str_option;

/* ---- Macros -------------------------------------------------------------- */

#define PPE_STR_MAX_SIZE (1L << (sizeof(ppe_size) * 8 - 1))

#define PPE_STR_SPACES " \t"

#define PPE_STR_WINDOWS_NEWLINE "\r\n"
#define PPE_STR_UNIX_NEWLINE "\n"

#define PPE_STR_UNLIMITED (-1)

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

PPE_API extern ppe_bool ppe_cspt_get(ppe_cs_snippet const restrict spt, const ppe_uint idx, ppe_cstr_c * const restrict s, ppe_size * restrict sz);
PPE_API extern ppe_bool ppe_cspt_append(ppe_cs_snippet restrict spt, ppe_cstr_c const restrict s, const ppe_size sz);

/* -- Producer -- */

PPE_API extern ppe_cstr_c ppe_cspt_copy(ppe_cs_snippet restrict spt, const ppe_uint idx, ppe_cstr restrict b, ppe_size * restrict bsz);
PPE_API extern ppe_cstr_c * ppe_cspt_copy_some(const ppe_cs_snippet restrict spt, const ppe_uint idx, ppe_uint * restrict n);

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

PPE_API extern ppe_cstr_c ppe_cs_get_empty(void);

/* -- Property -- */

PPE_API extern ppe_size ppe_cs_size(ppe_cstr_c const restrict s);

/* -- Test -- */

PPE_API extern ppe_bool ppe_cs_is_empty(ppe_cstr_c const restrict s);
PPE_API extern ppe_int ppe_cs_compare(ppe_cstr_c const restrict s1, ppe_cstr_c const restrict s2);

/* -- Find -- */

PPE_API extern ppe_cstr_c const ppe_cs_find(ppe_cstr_c const restrict s, ppe_cstr_c const restrict t);

/* -- Create & Destroy -- */

PPE_API extern void ppe_cs_destroy(ppe_cstr_c restrict s);
PPE_API extern ppe_cstr_c ppe_cs_create(ppe_cstr_c const restrict s, const ppe_size sz);

/* -- Substring -- */

PPE_API extern ppe_cstr_c ppe_cs_substr(ppe_cstr_c const restrict s, const ppe_size off, const ppe_size ssz, ppe_cstr restrict b, ppe_size * bsz, ppe_str_option opt);

/* -- Trim & Chomp -- */

PPE_API extern ppe_cstr_c ppe_cs_trim(ppe_cstr_c const restrict s, ppe_cstr_c const restrict t, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt);
PPE_API extern ppe_cstr_c ppe_cs_chop(ppe_cstr_c const restrict s, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt);
PPE_API extern ppe_cstr_c ppe_cs_chomp(ppe_cstr_c const restrict s, ppe_cstr_c const restrict t, const ppe_int n, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt);

/* -- Join & Concat -- */

PPE_API extern ppe_cstr_c ppe_cs_join(ppe_cstr_c const restrict d, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt, ...);
PPE_API extern ppe_cstr_c ppe_cs_concat(ppe_cstr restrict b, ppe_size * restrict bsz, ppe_str_option opt, ...);

/* -- Split -- */

PPE_API extern ppe_cs_snippet ppe_cs_split(const ppe_cstr restrict s, const ppe_cstr restrict d, ppe_cs_snippet restrict spt, ppe_uint * restrict n, ppe_cstr_c * restrict ss, const ppe_str_option opt);

/* -- Replace & Substitute -- */

PPE_API extern ppe_cstr_c ppe_cs_replace(ppe_cstr_c const restrict s, const ppe_size off, const ppe_size ssz, ppe_cstr_c const restrict to, ppe_cstr restrict b, ppe_size * restrict bsz, const ppe_str_option opt);
PPE_API extern ppe_cstr_c ppe_cs_substitute(ppe_cstr_c const restrict s, ppe_cstr_c const restrict from, ppe_cstr_c const restrict to, ppe_cstr restrict b, ppe_size * restrict bsz, ppe_uint * restrict n, const ppe_str_option opt);

/* -- Format -- */

PPE_API extern ppe_cstr ppe_cs_sprintf(ppe_cstr restrict b, ppe_size * restrict bsz, const ppe_str_option opt, const ppe_cstr restrict fmt, ...);

/* -- Wrapper -- */

static inline ppe_bool ppe_cs_equal_to(ppe_cstr_c const restrict s1, ppe_cstr_c const restrict s2)
{
    return ppe_cs_compare(s1, s2) == 0;
}

static inline ppe_bool ppe_cs_is_less_than(ppe_cstr_c const restrict s1, ppe_cstr_c const restrict s2)
{
    return ppe_cs_compare(s1, s2) < 0;
}

static inline ppe_bool ppe_cs_is_less_than_or_equal_to(ppe_cstr_c const restrict s1, ppe_cstr_c const restrict s2)
{
    return ppe_cs_compare(s1, s2) <= 0;
}

static inline ppe_bool ppe_cs_is_greater_than(ppe_cstr_c const restrict s1, ppe_cstr_c const restrict s2)
{
    return ppe_cs_compare(s1, s2) > 0;
}

static inline ppe_bool ppe_cs_is_greater_than_or_equal_to(ppe_cstr_c const restrict s1, ppe_cstr_c const restrict s2)
{
    return ppe_cs_compare(s1, s2) >= 0;
}

static inline ppe_cstr_c ppe_cs_clone(ppe_cstr_c const restrict s)
{
    return ppe_cs_create(s, ppe_cs_size(s));
}

static inline ppe_cstr_c ppe_cs_trim_left(ppe_cstr_c const restrict s, const ppe_cstr restrict t, ppe_cstr restrict b, ppe_size * bsz)
{
    return ppe_cs_trim(s, PPE_STR_SPACES, b, bsz, PPE_STR_OPT_LEFT_END);
}

static inline ppe_cstr_c ppe_cs_trim_right(ppe_cstr_c const restrict s, const ppe_cstr restrict t, ppe_cstr restrict b, ppe_size * bsz)
{
    return ppe_cs_trim(s, PPE_STR_SPACES, b, bsz, PPE_STR_OPT_RIGHT_END);
}

static inline ppe_cstr_c ppe_cs_trim_both(ppe_cstr_c const restrict s, ppe_cstr restrict b, ppe_size * bsz)
{
    return ppe_cs_trim(s, PPE_STR_SPACES, b, bsz, PPE_STR_OPT_BOTH_ENDS);
}

/* ==== Declaration : String ================================================ */

/* ---- Functions ----------------------------------------------------------- */

/* -- Preset values -- */

PPE_API extern const ppe_string ppe_str_get_empty(void);

/* -- Property -- */

PPE_API extern ppe_cstr_c ppe_str_addr(ppe_string restrict s);
PPE_API extern ppe_ssize ppe_str_size(ppe_string restrict s);

/* -- Test -- */

PPE_API extern ppe_bool ppe_str_is_empty(ppe_string restrict s);

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

PPE_API extern ppe_string ppe_str_create(ppe_cstr_c const restrict s, const ppe_size sz);
PPE_API extern ppe_string ppe_str_clone(ppe_string const restrict s);
PPE_API extern void ppe_str_destroy(ppe_string restrict s);

/* -- Manipulate -- */

PPE_API extern ppe_string ppe_str_substr(const ppe_string restrict s, const ppe_size off, const ppe_size rsz, const ppe_str_option opt);

/* ---- */

PPE_API extern ppe_string ppe_str_trim_ex(const ppe_string restrict s, const ppe_str_option opt, const void * restrict v, const ppe_size vsz);

static inline ppe_string ppe_str_trim_left(const ppe_string restrict s, const ppe_string restrict t, const ppe_str_option opt)
{
    return ppe_str_trim_ex(s, PPE_STR_OPT_LEFT_END, (void *) t, 0);
} /* ppe_str_trim_left */

static inline ppe_string ppe_str_trim_right(const ppe_string restrict s, const ppe_string restrict t, const ppe_str_option opt)
{
    return ppe_str_trim_ex(s, PPE_STR_OPT_RIGHT_END, (void *) t, 0);
} /* ppe_str_trim_right */

static inline ppe_string ppe_str_trim(const ppe_string restrict s, const ppe_string restrict t, const ppe_str_option opt)
{
    return ppe_str_trim_ex(s, PPE_STR_OPT_BOTH_ENDS, (void *) t, 0);
} /* ppe_str_trim */

static inline ppe_string ppe_str_trim_left_cs(const ppe_string restrict s, const ppe_cstr restrict t, const ppe_str_option opt)
{
    return ppe_str_trim_ex(s, PPE_STR_OPT_LEFT_END | PPE_STR_OPT_CSTR_ARG, (void *) t, 0);
} /* ppe_str_trim_left_cs */

static inline ppe_string ppe_str_trim_right_cs(const ppe_string restrict s, const ppe_cstr restrict t, const ppe_str_option opt)
{
    return ppe_str_trim_ex(s, PPE_STR_OPT_RIGHT_END | PPE_STR_OPT_CSTR_ARG, (void *) t, 0);
} /* ppe_str_trim_right_cs */

static inline ppe_string ppe_str_trim_cs(const ppe_string restrict s, const ppe_cstr restrict t, const ppe_str_option opt)
{
    return ppe_str_trim_ex(s, PPE_STR_OPT_BOTH_ENDS | PPE_STR_OPT_CSTR_ARG, (void *) t, 0);
} /* ppe_str_trim_cs */

/* ---- */

PPE_API extern ppe_string ppe_str_chop(const ppe_string restrict s, const ppe_str_option opt);
PPE_API extern ppe_string ppe_str_chomp_ex(const ppe_string restrict s, const ppe_str_option opt, const void * restrict v, const ppe_size vsz);

static inline ppe_string ppe_str_chomp_newline(const ppe_string restrict s, const ppe_str_option opt)
{
    return ppe_str_chomp_ex(s, (opt | PPE_STR_OPT_CSTR_ARG), NULL, 0);
} /* ppe_str_chomp_newline */

static inline ppe_string ppe_str_chomp(const ppe_string restrict s, const ppe_string restrict t, const ppe_str_option opt)
{
    return ppe_str_chomp_ex(s, (opt & (~PPE_STR_OPT_CSTR_ARG)), (void *) t, 0);
} /* ppe_str_chomp */

static inline ppe_string ppe_str_chomp_cs(const ppe_string restrict s, const ppe_cstr restrict t, const ppe_str_option opt)
{
    return ppe_str_chomp_ex(s, (opt | PPE_STR_OPT_CSTR_ARG), (void *) t, ppe_cs_size(t));
} /* ppe_str_chomp_cs */

/* -- Join & Concat -- */

PPE_API extern ppe_string ppe_str_join(const ppe_string restrict d, const ppe_str_option opt, ...);
PPE_API extern ppe_string ppe_str_concat(const ppe_str_option opt, ...);

/* -- Split -- */

PPE_API extern ppe_cs_snippet ppe_str_split(const ppe_string restrict s, const ppe_string restrict d, ppe_cs_snippet restrict spt, ppe_uint * restrict n, const ppe_str_option opt);
PPE_API extern ppe_cs_snippet ppe_str_split_cs(const ppe_string restrict s, const ppe_cstr restrict d, ppe_cs_snippet restrict spt, ppe_uint * restrict n, const ppe_str_option opt);

/* -- Replace & Substitute -- */

PPE_API extern ppe_string ppe_str_replace(const ppe_string restrict s, const ppe_size off, const ppe_size ssz, const ppe_string restrict to, const ppe_str_option opt);
PPE_API extern ppe_string ppe_str_replace_cs(const ppe_string restrict s, const ppe_size off, const ppe_size ssz, const ppe_cstr restrict to, const ppe_str_option opt);

/* ---- */

PPE_API extern ppe_string ppe_str_substitute(const ppe_string restrict s, const ppe_string restrict from, const ppe_string restrict to, ppe_uint * restrict n, const ppe_str_option opt);
PPE_API extern ppe_string ppe_str_substitute_cs(const ppe_string restrict s, const ppe_cstr restrict from, const ppe_cstr restrict to, ppe_uint * restrict n, const ppe_str_option opt);

/* -- Wrapper -- */

static inline ppe_string ppe_str_clone_cs(ppe_cstr_c const restrict s)
{
    return ppe_str_create(s, ppe_cs_size(s));
} /* ppe_str_clone_cs */

static inline ppe_string ppe_str_join_two(const ppe_string restrict d, const ppe_string restrict s1, const ppe_string restrict s2)
{
    return ppe_str_join(d, PPE_STR_OPT_NONE, PPE_STR_JOIN_ADD_ITEM_STRING, s1, PPE_STR_JOIN_ADD_ITEM_STRING, s2, PPE_STR_JOIN_END);
}

static inline ppe_string ppe_str_concat_two(const ppe_string restrict s1, const ppe_string restrict s2)
{
    return ppe_str_join(ppe_str_get_empty(), PPE_STR_OPT_NONE, PPE_STR_JOIN_ADD_ITEM_STRING, s1, PPE_STR_JOIN_ADD_ITEM_STRING, s2, PPE_STR_JOIN_END);
}

#ifdef __cplusplus
}
#endif

#endif /* PPE_STRING_H */

