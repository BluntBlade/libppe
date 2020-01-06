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

typedef enum
{
    PPE_PTR_JOIN_END = 0,
    PPE_PTR_JOIN_UNSET_DELIMITER = 1,
    PPE_PTR_JOIN_SET_DELIMITER_CSTR = 2,
    PPE_PTR_JOIN_SET_DELIMITER_CSTR_WITH_SIZE = 3,
    PPE_PTR_JOIN_SET_DELIMITER_STRING = 4,
    PPE_PTR_JOIN_ADD_ITEM_CSTR = 5,
    PPE_PTR_JOIN_ADD_ITEM_CSTR_WITH_SIZE = 6,
    PPE_PTR_JOIN_ADD_ITEM_STRING = 7
} ppe_str_join_action;

typedef enum
{
    PPE_STR_OPT_DIRECT_LEFT = 0x00000001,
    PPE_STR_OPT_DIRECT_RIGHT = 0x00000002,
    PPE_STR_OPT_DIRECT_BOTH = 0x00000003,
    PPE_STR_OPT_IN_PLACE = 0x40000000,
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

PPE_API extern ppe_cstr ppe_cs_substr(const ppe_cstr restrict s, const ppe_size index, const ppe_size bytes);

/* -- Trim & Chomp -- */

PPE_API extern ppe_cstr ppe_cs_trim_bytes(ppe_cstr restrict s, const ppe_cstr restrict accept, ppe_str_option opt);
PPE_API extern ppe_cstr ppe_cs_chop(ppe_cstr restrict s, ppe_str_option opt);
PPE_API extern ppe_cstr ppe_cs_chomp(ppe_cstr restrict s, ppe_str_option opt);

/* -- Join & Concat -- */

PPE_API extern ppe_cstr ppe_cs_join(ppe_char * restrict b, ppe_size * restrict bsz, const ppe_cstr restrict d, ppe_str_option opt, ...);
PPE_API extern ppe_cstr ppe_cs_concat(ppe_char * restrict b, ppe_size * restrict bsz, ppe_str_option opt, ...);

/* -- Split & Slice -- */

/* Slice the substring which is before the delimiter in the source string.
 *
 * Parameters:
 *      b    A buffer which will hold the substring. If pass a NULL, detect and save the size of the substring into the parameter `bsz`. Include no terminating NUL character.
 *      bsz  A pointer to the size variable of the buffer `b`. **MUST NOT** be a NULL.
 *      d    A non-emtpy string used as a delimiter. **MUST NOT** be a NULL.
 *      s    The source string in which the delimiter is being found. **MUST NOT** be a NULL.
 *
 * Return values:
 *      ppe_true    The substring is copyed into the buffer, and the size of it is saved in the bsz.
 *      ppe_false   Something wrong happened.
 *
 * Errors:
 *      PPE_ERR_INVALID_ARGUMENT    The delmiter string is empty.
 *      PPE_ERR_OUT_OF_BUFFER       No enough space in the buffer.
 */
PPE_API extern ppe_bool ppe_cs_slice_into(ppe_char * restrict b, ppe_size * restrict bsz, const ppe_cstr restrict d, const ppe_cstr restrict s);
/* TODO: ppe_bool ppe_cs_split(ppe_cs_array * restrict a, const ppe_cstr restrict d, const ppe_cstr restrict s); */

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

static inline ppe_cstr ppe_cs_trim_left(const ppe_cstr restrict s, const ppe_cstr restrict accept)
{
    return ppe_cs_trim_bytes(s, PPE_STR_SPACES, PPE_STR_OPT_DIRECT_LEFT);
}

static inline ppe_cstr ppe_cs_trim_right(const ppe_cstr restrict s, const ppe_cstr restrict accept)
{
    return ppe_cs_trim_bytes(s, PPE_STR_SPACES, PPE_STR_OPT_DIRECT_RIGHT);
}

static inline ppe_cstr ppe_cs_trim(const ppe_cstr restrict s, const ppe_cstr restrict accept)
{
    return ppe_cs_trim_bytes(s, PPE_STR_SPACES, PPE_STR_OPT_DIRECT_BOTH);
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

/* -- Join -- */

PPE_API extern ppe_string ppe_str_join(const ppe_string restrict d, ...);

/* TODO: ppe_str_concat() as a macro with variable arguments. Need C99 support. */

/* split */

PPE_API extern struct ppe_str_bunch_st * ppe_cs_split(const ppe_char * restrict d, const ppe_ssize dsz, const ppe_char * restrict s, const ppe_ssize sz, const ppe_int n);
PPE_API extern struct ppe_str_bunch_st * ppe_cs_split_str(const ppe_char * restrict d, const ppe_ssize dsz, const ppe_string restrict s, const ppe_int n);

/* _str_ series */

/* split */

PPE_API extern struct ppe_str_bunch_st * ppe_str_split_cstr(const ppe_string restrict d, const ppe_char * restrict s, const ppe_ssize sz, const ppe_int n);

/* -- Formatting -- */

/* PPE_API extern ppe_ssize ppe_misc_snprintf(ppe_char * restrict buf, ppe_ssize * restrict buf_size, const ppe_char * restrict format, ...); */
PPE_API extern ppe_string ppe_cs_vsprintf(const ppe_char * restrict fmt, va_list args);

/* -- Wrapper -- */

static ppe_string ppe_cs_sprintf(const ppe_char * restrict fmt, ...)
{
    ppe_string new_str = NULL;
    va_list args;

    va_start(args, fmt);
    new_str = ppe_cs_vsprintf(fmt, args);
    va_end(args);
    return new_str;
}

static inline struct ppe_str_bunch_st * ppe_str_split(const ppe_string restrict d, const ppe_string restrict s, const ppe_int n)
{
    return ppe_str_split_cstr(d, ppe_str_addr(s), ppe_str_size(s), n);
}

static inline struct ppe_string ppe_str_join_two(const ppe_string restrict d, const ppe_string restrict s1, const ppe_string restrict s2)
{
    return ppe_str_join(d, PPE_PTR_JOIN_ADD_ITEM_STRING, s1, PPE_PTR_JOIN_ADD_ITEM_STRING, s2, PPE_PTR_JOIN_END);
}

static inline struct ppe_string ppe_str_concat_two(const ppe_string restrict s1, const ppe_string restrict s2)
{
    return ppe_str_join(ppe_str_empty(), PPE_PTR_JOIN_ADD_ITEM_STRING, s1, PPE_PTR_JOIN_ADD_ITEM_STRING, s2, PPE_PTR_JOIN_END);
}

#ifdef __cplusplus
}
#endif

#endif /* PPE_STRING_H */

