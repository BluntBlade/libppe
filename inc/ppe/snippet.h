#ifndef PPE_SNIPPET_H
#define PPE_SNIPPET_H 1

#include "ppe/types.h"
#include "ppe/macros.h"
#include "ppe/string.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct PPE_SNIPPET;
typedef struct PPE_SNIPPET * ppe_snippet;
typedef const ppe_snippet ppe_snippet_c;

/* ==== Declaration : C-String Snippet ====================================== */

/* -- Property -- */

PPE_API extern ppe_uint ppe_spt_count(ppe_snippet_c restrict spt);
PPE_API extern ppe_uint ppe_spt_capacity(ppe_snippet_c restrict spt);
PPE_API extern ppe_size ppe_spt_total_size(ppe_snippet_c restrict spt);

/* -- Create & Destroy -- */

PPE_API extern ppe_snippet ppe_spt_create(const ppe_uint cap);
PPE_API extern void ppe_spt_destroy(ppe_snippet restrict spt);
PPE_API extern void ppe_spt_reset(ppe_snippet restrict spt);

/* -- Manipulators -- */

PPE_API extern ppe_bool ppe_spt_points_to(ppe_snippet restrict spt, ppe_cstr_c restrict s, const ppe_size sz);
PPE_API extern ppe_cstr_c ppe_spt_measure(ppe_snippet_c restrict spt, const ppe_uint idx, ppe_size * restrict sz);
PPE_API extern ppe_cstr_c ppe_spt_duplicate(ppe_snippet_c restrict spt, const ppe_uint idx);
PPE_API extern ppe_cstr_c ppe_spt_copy(ppe_snippet_c restrict spt, const ppe_uint idx, ppe_cstr restrict b, ppe_size * restrict bsz);

/* -- Wrappers -- */

static inline ppe_bool ppe_spt_is_empty(ppe_snippet_c restrict spt)
{
    return ppe_spt_count(spt) == 0;
} /* ppe_spt_is_empty */

static inline ppe_bool ppe_spt_is_full(ppe_snippet_c restrict spt)
{
    return ppe_spt_count(spt) == ppe_spt_capacity(spt);
} /* ppe_spt_is_full */

#ifdef __cplusplus
}
#endif

#endif /* PPE_SNIPPET_H */

