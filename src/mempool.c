#include "ppe/errors.h"
#include "ppe/mempool.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct PPE_MEMPOOL
{
    ppe_mp_allocate_vfn alloc;
    ppe_mp_reallocate_vfn realloc;
    ppe_mp_deallocate_vfn free;
} ppe_mempool_st;

PPE_API ppe_mempool ppe_mp_create(ppe_mp_allocate_vfn restrict alloc, ppe_mp_reallocate_vfn restrict realloc, ppe_mp_deallocate_vfn restrict free)
{
    ppe_mempool mp = calloc(sizeof(1, ppe_mempool_st));
    if (! mp) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    mp->alloc = alloc;
    mp->realloc = realloc;
    mp->free = free;
    return mp;
}

PPE_API void ppe_mp_destroy(ppe_mempool restrict mp)
{
    if (mp) {
        free(mp);
    }
}

PPE_API void * ppe_mp_allocate(ppe_mempool_itf restrict itf, ppe_size size)
{
    return itf ? itf->alloc(itf, size) : malloc(size);
}

PPE_API void * ppe_mp_reallocate(ppe_mempool_itf restrict itf, void * restrict ptr, ppe_size size)
{
    return itf ? itf->realloc(itf, size) : realloc(ptr, size);
}

PPE_API void ppe_mp_deallocate(ppe_mempool_itf restrict itf, void * restrict ptr)
{
    if (itf) {
        itf->free(itf, ptr); 
    } else {
        free(ptr);
    }
}

#ifdef __cplusplus
}
#endif
