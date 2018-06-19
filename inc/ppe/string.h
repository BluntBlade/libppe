#ifndef PPE_STRING_H
#define PPE_STRING_H 1 

#include "ppe/types.h"
#include "ppe/macros.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct ppe_string_st; 
typedef struct ppe_string_st * ppe_string;

PPE_API extern ppe_string ppe_str_clone(const ppe_string restrict str);
PPE_API extern ppe_string ppe_str_clone_raw(const ppe_char * restrict str);
PPE_API extern ppe_string ppe_str_clone_raw2(const ppe_char * restrict str, ppe_size len);

PPE_API extern ppe_string ppe_str_join(const ppe_char * restrict deli, const ppe_string restrict str1, const ppe_string restrict str2, ...);
PPE_API extern ppe_string ppe_str_join_list(const ppe_char * restrict deli, const ppe_string * restrict strs, int n);
PPE_API extern ppe_string ppe_str_join_raw(const ppe_char * restrict deli, const ppe_char * restrict str1, const ppe_char * restrict str2, ...);
PPE_API extern ppe_string ppe_str_join_raw2(const ppe_char * restrict deli, const ppe_char * restrict str1, ppe_size len1, const ppe_char * restrict str2, ppe_size len2, ...);
PPE_API extern ppe_string ppe_str_join_raw_list(const ppe_char * restrict deli, const ppe_char ** restrict strs, int n);

PPE_API extern ppe_string ppe_str_concat(const ppe_string restrict str1, const ppe_string restrict str2, ...);
PPE_API extern ppe_string ppe_str_concat_list(const ppe_string * restrict strs, int n);
PPE_API extern ppe_string ppe_str_concat_raw(const ppe_char * restrict str1, const ppe_char * restrict str2, ...);
PPE_API extern ppe_string ppe_str_concat_raw2(const ppe_char * restrict str1, ppe_size len1, const ppe_char * restrict str2, ppe_size len2, ...);
PPE_API extern ppe_string ppe_str_concat_raw_list(const ppe_char ** restrict strs, const int n);

PPE_API extern ppe_bool ppe_str_snprintf(ppe_char * restrict buf, ppe_size * restrict buf_size, const ppe_char * restrict format, ...);
PPE_API extern ppe_string ppe_str_sprintf(const ppe_char * restrict format, ...);

#ifdef __cplusplus
}
#endif

#endif /* PPE_STRING_H */
