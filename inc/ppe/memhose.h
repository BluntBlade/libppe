#ifndef PPE_MEMHOSE_H
#define PPE_MEMHOSE_H 1

#include "ppe/types.h"
#include "ppe/macros.h"

#include "ppe/mempool.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ---- Types ---- */

struct PPE_MEMHOSE;
typedef struct PPE_MEMHOSE * ppe_memhose;

/* ---- Functions ---- */

PPE_API extern ppe_memhose ppe_mhs_create(ppe_size min_capacity);
PPE_API extern void ppe_mhs_destroy(ppe_memhose restrict hose);
PPE_API extern void ppe_mhs_free_all(ppe_memhose restrict hose);

PPE_API extern void * ppe_mhs_malloc(ppe_memhose restrict hose, ppe_size size);
PPE_API extern void * ppe_mhs_calloc(ppe_memhose restrict hose, ppe_size num, ppe_size size);
PPE_API extern void * ppe_mhs_realloc(ppe_memhose restrict hose, void * restrict ptr, ppe_size size);
PPE_API extern void ppe_mhs_free(ppe_memhose restrict hose, void * restrict ptr);

PPE_API extern ppe_mempool_itf ppe_mhs_to_mempool(ppe_memhose restrict hose);

#ifdef __cplusplus
}
#endif

#endif /* PPE_MEMHOSE_H */

