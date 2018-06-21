#ifndef PPE_MEMPOOL_H
#define PPE_MEMPOOL_H 1

#include "ppe/types.h"
#include "ppe/macros.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct PPE_MEMPOOL;
typedef struct PPE_MEMPOOL * ppe_mempool;
typedef struct PPE_MEMPOOL ** ppe_mempool_itf;

typedef void * (*ppe_mp_allocate_vfn)(ppe_mempool_itf restrict itf, ppe_size size);
typedef void * (*ppe_mp_reallocate_vfn)(ppe_mempool_itf restrict itf, void * restrict ptr, ppe_size size);
typedef void (*ppe_mp_deallocate_vfn)(ppe_mempool_itf restrict itf, void * restrict ptr);

PPE_API ppe_mempool ppe_mp_create(ppe_mp_allocate_vfn restrict alloc, ppe_mp_reallocate_vfn restrict realloc, ppe_mp_free_vfn restrict free);
PPE_API void ppe_mp_destroy(ppe_mempool restrict mp);

PPE_API void * ppe_mp_allocate(ppe_mempool_itf restrict itf, ppe_size size);
PPE_API void * ppe_mp_reallocate(ppe_mempool_itf restrict itf, void * restrict ptr, ppe_size size);
PPE_API void ppe_mp_deallocate(ppe_mempool_itf restrict itf, void * restrict ptr);

static inline void * ppe_mp_malloc(ppe_size size)
{
    return ppe_mp_allocate(NULL, size);
}

static inline void * ppe_mp_realloc(void * restrict ptr, ppe_size size)
{
    return ppe_mp_reallocate(NULL, ptr, size);
}

static inline void ppe_mp_free(void * restrict ptr)
{
    return ppe_mp_deallocate(NULL, ptr);
}

#ifdef __cplusplus
}
#endif

#endif /* PPE_MEMPOOL_H */
