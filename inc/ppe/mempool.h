#ifndef PPE_MEMPOOL_H
#define PPE_MEMPOOL_H 1

#include "ppe/types.h"
#include "ppe/macros.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ---- Types ---- */

struct PPE_MEMPOOL;
typedef struct PPE_MEMPOOL * ppe_mempool;
typedef struct PPE_MEMPOOL ** ppe_mempool_itf;

typedef void * (*ppe_mp_malloc_vfn)(ppe_mempool_itf restrict itf, ppe_size size);
typedef void * (*ppe_mp_realloc_vfn)(ppe_mempool_itf restrict itf, void * restrict ptr, ppe_size size);
typedef void (*ppe_mp_free_vfn)(ppe_mempool_itf restrict itf, void * restrict ptr);

/* ---- Preset Values ---- */

const ppe_mempool_itf PPE_MP_GLOBAL_ALLOCATOR = NULL:

/* ---- Functions ---- */

PPE_API extern ppe_mempool ppe_mp_create(ppe_mp_malloc_vfn restrict malloc, ppe_mp_realloc_vfn restrict realloc, ppe_mp_free_vfn restrict free);
PPE_API extern void ppe_mp_destroy(ppe_mempool restrict mp);

PPE_API extern void * ppe_mp_malloc_from(ppe_mempool_itf restrict itf, ppe_size size);
PPE_API extern void * ppe_mp_calloc_from(ppe_mempool_itf restrict itf, ppe_size num, ppe_size size);
PPE_API extern void * ppe_mp_realloc_from(ppe_mempool_itf restrict itf, void * restrict ptr, ppe_size size);
PPE_API extern void ppe_mp_free_to(ppe_mempool_itf restrict itf, void * restrict ptr);

PPE_API extern ppe_mempool_itf ppe_mp_set_default(ppe_mempool_itf restrict itf);
PPE_API extern ppe_mempool_itf ppe_mp_get_default(void);

/* ---- Wrappers ---- */

PPE_API extern void * ppe_mp_malloc(ppe_size size);
PPE_API extern void * ppe_mp_calloc(ppe_size num, ppe_size size);
PPE_API extern void * ppe_mp_realloc(void * restrict ptr, ppe_size size)
PPE_API extern void ppe_mp_free(void * restrict ptr);

#ifdef __cplusplus
}
#endif

#endif /* PPE_MEMPOOL_H */
