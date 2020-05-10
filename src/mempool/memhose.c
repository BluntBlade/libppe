#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "ppe/error.h"
#include "ppe/mempool/memhose.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ---- Types ---- */

typedef struct PPE_MEMHOSE_BULK
{
    ppe_size cap;
    ppe_size unused;
    void * buf;
} ppe_memhose_bulk_st, *ppe_memhose_bulk_ptr;

struct PPE_MEMHOSE
{
    ppe_mempool mempool;

    ppe_uint blk_end;
    ppe_uint blk_cnt;
    ppe_memhose_bulk_st * blks;
    ppe_memhose_bulk_st blks_init[1];
} ppe_memhose_st;

/* ---- Preset Values ---- */

const ppe_size ppe_mhs_bulk_size = 4 * (1 << 20);       /* 4MB for bulk size */

/* ---- Functions ---- */

static void * ppe_mhs_malloc_cfn(ppe_mempool_itf restrict itf, ppe_size size)
{
    return ppe_mhs_malloc((ppe_memhose)(itf), size);
}

static void * ppe_mhs_realloc_cfn(ppe_mempool_itf restrict itf, void * restrict ptr, ppe_size size)
{
    return ppe_mhs_realloc((ppe_memhose)(itf), ptr, size);
}

static void ppe_mhs_free_cfn(ppe_mempool_itf restrict itf, void * restrict ptr)
{
    ppe_mhs_free((ppe_memhose)(itf), ptr);
}

static inline ppe_size ppe_mhs_round_up(ppe_size size)
{
    if (size & (ppe_size)(sizeof(void*) - 1)) {
        return (size + sizeof(void*)) & (~ (ppe_size)(sizeof(void*) - 1));
    }
    return size;
}

static inline ppe_uint ppe_mhs_bulk_count(ppe_size cap)
{
    return (cap / ppe_mhs_bulk_size) + (cap & (ppe_mhs_bulk_size - 1) ? 1 : 0);
}

PPE_API ppe_memhose ppe_mhs_create(ppe_size min_capacity)
{
    static ppe_mempool mempool = NULL;
    ppe_memhose hose = NULL;
    ppe_size new_cap = 0;

    if (! mempool && ! (mempool = ppe_mp_create(&ppe_mhs_malloc_cfn, &ppe_mhs_realloc_cfn, &ppe_mhs_free_cfn))) return NULL;

    if (! (hose = (ppe_memhose)(calloc(1, sizeof(ppe_memhose_st))))) {
        /* No need to destroy the mempool */
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    new_cap = (min_capacity == 0 ? ppe_mhs_bulk_size : ppe_mhs_round_up(min_capacity));
    if (! (hose->blks_init[0].buf = malloc(new_cap))) {
        free(hose);
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    hose->mempool = mempool;

    hose->blk_end = 1;
    hose->blk_cnt = 1;
    hose->blks = &hose->blks_init[0];
    hose->blks_init[0].cap = new_cap;
    hose->blks_init[0].unused = new_cap;
    return hose;
}

PPE_API void ppe_mhs_destroy(ppe_memhose restrict hose)
{
    ppe_uint i = 0;

    if (hose) {
        for (i = 0; i < hose->blk_cnt; i++) free(hose->blks[i].buf);

        if (hose->blks != &hose->blks_init[0]) {
            memset(hose->blks, 0, sizeof(ppe_memhose_bulk_st) * hose->blk_cnt);
            free(hose->blks);
        }

        memset(hose, 0, sizeof(ppe_memhose_st));
        free(hose);
    }
}

PPE_API void ppe_mhs_free_all(ppe_memhose restrict hose)
{
    ppe_uint i = 0;

    assert(hose);

    for (i = 0; i < hose->blk_cnt; i++) hose->blks[i].unused = hose->blks[i].cap;
    hose->blk_end = hose->blk_cnt;
}

static ppe_bool ppe_mhs_augment(ppe_memhose restrict hose, ppe_size total)
{
    ppe_memhose_bulk_st * new_blks = NULL;
    void * new_buf = NULL;
    ppe_size new_cap = (ppe_mhs_bulk_size < total ? total * 2 : ppe_mhs_bulk_size);

    if (! (new_blks = calloc(hose->blk_cnt + 1, sizeof(ppe_memhose_bulk_st)))) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return ppe_false;
    }
    if (! (new_buf = malloc(new_cap))) {
        free(new_blks);
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return ppe_false;
    }
    memcpy(new_blks + sizeof(ppe_memhose_bulk_st), hose->blks, sizeof(ppe_memhose_bulk_st) * hose->blk_cnt);
    new_blks[0].cap = new_cap;
    new_blks[0].unused = new_cap;
    new_blks[0].buf = new_buf;

    memset(hose->blks, 0, sizeof(ppe_memhose_bulk_st) * hose->blk_cnt);
    if (hose->blks != &hose->blks_init[0]) free(hose->blks);

    hose->blks = new_blks;
    hose->blk_end += 1;
    hose->blk_cnt += 1;
    return ppe_true;
}

static inline void ppe_mhs_swap_bulk(ppe_memhose restrict hose, ppe_uint i, ppe_uint j)
{
    ppe_memhose_bulk_st blk = hose->blks[i];
    hose->blks[i] = hose->blks[j];
    hose->blks[j] = blk;
}

PPE_API void * ppe_mhs_malloc(ppe_memhose restrict hose, ppe_size size)
{
    ppe_uint i = 0;
    void * ptr = NULL;
    ppe_size total = ppe_mhs_round_up(size == 0 ? 1 : size);

    assert(hose);

    for (i = 0; i < hose->blk_end; i++) {
        if (hose->blks[i].unused < total) continue;

        ptr = hose->blks[i].buf + (hose->blks[i].cap - hose->blks[i].unused);
        hose->blks[i].unused -= total;

        if (hose->blks[i].unused < sizeof(void*)) ppe_mhs_swap_bulk(hose, i, --hose->blk_end);
        return ptr;
    }

    if (! ppe_mhs_augment(hose, total)) return NULL;
    ptr = hose->blks[0].buf + (hose->blks[0].cap - hose->blks[0].unused);
    hose->blks[0].unused -= total;
    return ptr;
}

PPE_API void * ppe_mhs_calloc(ppe_memhose restrict hose, ppe_size num, ppe_size size)
{
    void * p = ppe_mhs_malloc(hose, num * size);
    if (p) memset(p, 0, num * size);
    return p;
}

PPE_API void * ppe_mhs_realloc(ppe_memhose restrict hose, void * restrict ptr, ppe_size size)
{
    return ppe_mhs_malloc(hose, size);
}

PPE_API void ppe_mhs_free(ppe_memhose restrict hose, void * restrict ptr)
{
    assert(hose);
    return;
}

PPE_API ppe_mempool_itf ppe_mhs_to_mempool(ppe_memhose restrict hose)
{
    return &hose->mempool;
}

#ifdef __cplusplus
}
#endif
