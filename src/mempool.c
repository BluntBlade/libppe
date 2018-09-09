#include <stdlib.h>
#include <string.h>

#include "ppe/errors.h"
#include "ppe/mempool.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ---- Types ---- */

typedef struct PPE_MEMPOOL
{
    ppe_mp_malloc_vfn malloc;
    ppe_mp_realloc_vfn realloc;
    ppe_mp_free_vfn free;
} ppe_mempool_st;

/* ---- Functions ---- */

PPE_API ppe_mempool ppe_mp_create(ppe_mp_malloc_vfn malloc, ppe_mp_realloc_vfn realloc, ppe_mp_free_vfn free)
{
    ppe_mempool mp = calloc(1, sizeof(ppe_mempool_st));
    if (! mp) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    mp->malloc = malloc;
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

PPE_API void * ppe_mp_malloc_from(ppe_mempool_itf restrict itf, ppe_size size)
{
    return itf ? (*itf)->malloc(itf, size) : malloc(size);
}

PPE_API void * ppe_mp_calloc_from(ppe_mempool_itf restrict itf, ppe_size num, ppe_size size)
{
    void * p = NULL;
    if (! itf) return calloc(num, size);

    if ((p = (*itf)->malloc(itf, num * size))) memset(p, 0, num * size);
    return p;
}

PPE_API void * ppe_mp_realloc_from(ppe_mempool_itf restrict itf, void * restrict ptr, ppe_size size)
{
    return itf ? (*itf)->realloc(itf, ptr, size) : realloc(ptr, size);
}

PPE_API void ppe_mp_free_to(ppe_mempool_itf restrict itf, void * restrict ptr)
{
    if (itf) {
        (*itf)->free(itf, ptr); 
    } else {
        free(ptr);
    }
}

static ppe_mempool_itf s_mp_default = PPE_MP_GLOBAL_ALLOCATOR;

PPE_API ppe_mempool_itf ppe_mp_set_default(ppe_mempool_itf restrict itf)
{
    ppe_mempool_itf t = s_mp_default;
    s_mp_default = itf;
    return t;
}

PPE_API ppe_mempool_itf ppe_mp_get_default(void)
{
    return s_mp_default;
}

/* ---- Wrappers ---- */

PPE_API void * ppe_mp_malloc(ppe_size size)
{
    return ppe_mp_malloc_from(s_mp_default, size);
}

PPE_API void * ppe_mp_calloc(ppe_size num, ppe_size size)
{
    return ppe_mp_calloc_from(s_mp_default, num, size);
}

PPE_API void * ppe_mp_realloc(void * restrict ptr, ppe_size size)
{
    return ppe_mp_realloc_from(s_mp_default, ptr, size);
}

PPE_API void ppe_mp_free(void * restrict ptr)
{
    return ppe_mp_free_to(s_mp_default, ptr);
}

#ifdef __cplusplus
}
#endif
