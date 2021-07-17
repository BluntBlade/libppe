#include <string.h>
#include <assert.h>

#include "ppe/mempool.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ==== Declaration : C-String Snippet ====================================== */

/* -- Macro -- */

#define CSPT_PRESERVED_CAPACITY 4

/* ---- Types --------------------------------------------------------------- */

typedef struct SPT_ITEM {
    ppe_cstr_c s;
    ppe_size sz;
} spt_item_st;

typedef struct PPE_SNIPPET {
    ppe_uint cnt;
    ppe_uint cap;
    ppe_size total;
    spt_item_st items[CSPT_PRESERVED_CAPACITY];
} ppe_snippet_st;

/* ---- Functions ------------------------------------------------------- */

/* -- Property -- */

PPE_API ppe_uint ppe_spt_count(ppe_snippet_c restrict spt)
{
    return spt->cnt;
} /* ppe_spt_count */

PPE_API ppe_uint ppe_spt_capacity(ppe_snippet_c restrict spt)
{
    return spt->cap;
} /* ppe_spt_capacity */

PPE_API ppe_size ppe_spt_total_size(ppe_snippet_c restrict spt)
{
    return spt->total;
} /* ppe_spt_total_size */

/* -- Create & Destroy -- */

PPE_API ppe_snippet ppe_spt_create(const ppe_uint cap)
{
    ppe_snippet nw = NULL;
    ppe_size nwsz = sizeof(ppe_cs_snippet_st);

    nwsz += cap > CSPT_PRESERVED_CAPACITY ? sizeof(spt_item_st) * (cap - CSPT_PRESERVED_CAPACITY) : 0;
    nw = ppe_mp_malloc(nwsz);
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }
    nw->cnt = 0;
    nw->cap = cap;
    nw->total = 0;
    return nw;
} /* ppe_spt_create */

PPE_API void ppe_spt_destroy(ppe_snippet restrict spt)
{
    if (spt) {
        ppe_mp_free(spt);
    }
} /* ppe_spt_destroy */

PPE_API void ppe_spt_reset(ppe_snippet restrict spt)
{
    spt->cnt = 0;
    spt->total = 0;
} /* ppe_spt_reset */

/* -- Manipulators -- */

PPE_API ppe_bool ppe_spt_points_to(ppe_snippet restrict spt, ppe_cstr_c restrict s, const ppe_size sz)
{
    if (spt->cnt == spt->cap) {
        ppe_err_set(PPE_ERR_OUT_OF_CAPACITY, NULL);
        return ppe_false;
    }

    spt->items[spt->cnt].s = s;
    spt->items[spt->cnt].sz = sz;
    spt->cnt += 1;
    spt->total += sz;
    return ppe_true;
} /* ppe_spt_points_to */

PPE_API ppe_cstr_c ppe_spt_measure(ppe_snippet_c restrict spt, const ppe_uint idx, ppe_size * restrict sz)
{
    if (! sz) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (spt->cnt <= idx) {
        ppe_err_set(PPE_ERR_OUT_OF_RANGE, NULL);
        return ppe_false;
    }
    *sz = spt->items[idx].sz;
    return spt->items[idx].s;
} /* ppe_spt_measure */

PPE_API ppe_cstr_c ppe_spt_duplicate(ppe_snippet_c restrict spt, const ppe_uint idx)
{
    if (spt->cnt <= idx) {
        ppe_err_set(PPE_ERR_OUT_OF_RANGE, NULL);
        return NULL;
    }
    return ppe_cs_create(spt->items[idx].s, spt->items[idx].sz);
} /* ppe_spt_duplicate */

PPE_API ppe_cstr_c ppe_spt_copy(ppe_snippet_c restrict spt, const ppe_uint idx, ppe_cstr restrict b, ppe_size * restrict bsz)
{
    if (! b || ! bsz || *bsz < 0) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return NULL;
    }
    if (spt->cnt <= idx) {
        ppe_err_set(PPE_ERR_OUT_OF_RANGE, NULL);
        return NULL;
    }
    if (*bsz < spt->items[idx].sz) {
        ppe_err_set(PPE_ERR_OUT_OF_CAPACITY, NULL);
        return NULL;
    }

    memcpy(b, spt->items[idx].s, spt->items[idx].sz);
    *bsz = spt->items[idx].sz;
    return b;
} /* ppe_spt_copy */

#ifdef __cplusplus
}
#endif

