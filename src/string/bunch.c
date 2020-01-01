#include <assert.h>
#include "ppe/string/bunch.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ==== Definition : String Bunch ==== */

/* ---- Types ---- */

struct PPE_SBC_BUFFER;

typedef struct PPE_SBC_BUFFER
{
    char * pos;
    char * end;
    char * ptr;
} ppe_sbc_buffer_st, *ppe_sbc_buffer;

typedef struct PPE_STR_BUNCH
{
    struct {
        ppe_string_st * ent;
        ppe_uint i;             /* The count of used text snippets. */
        ppe_uint n;             /* The total amount of text snippets. */
    } ref;

    struct {
        ppe_sbc_buffer_st * ent;
        ppe_uint i;             /* The index of the current buffer in use. */
        ppe_uint n;             /* The total amount of buffers. */
        ppe_ssize nwcap;        /* The default capacity of new buffer. */
    } buf;

    ppe_ssize total;            /* The total size of referenced and buffered strings in bytes. */
    ppe_string_st strs[8];
    ppe_sbc_buffer_st bufs[2];
} ppe_str_bunch_st;

/* ---- Functions ---- */

#define PPE_SBC_BUFFER_SIZE 4096

static inline void ppe_sbc_init(ppe_str_bunch restrict bc)
{
    memset(bc, 0, sizeof(ppe_str_bunch_st));

    bc->ref.ent = &bc->strs[0];
    bc->ref.n = sizeof(bc->strs) / sizeof(bc->strs[0]);

    bc->buf.ent = &bc->bufs[0];
    bc->buf.nwcap = PPE_SBC_BUFFER_SIZE;
    bc->buf.n = sizeof(bc->bufs) / sizeof(bc->bufs[0]);
}

PPE_API ppe_str_bunch ppe_sbc_create(void)
{
    ppe_str_bunch nw = ppe_mp_malloc(sizeof(ppe_str_bunch_st));
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }
    ppe_sbc_init(nw);
    return nw;
}

static void ppe_sbc_clean(ppe_str_bunch restrict bc)
{
    ppe_uint i = 0;

    for (i = 0; i <= bc->buf.i; ++i) {
        ppe_mp_free(bc->buf.ent[i]->buf);
    }
    if (bc->ref.ent != &bc->strs[0]) {
        ppe_mp_free(bc->ref.ent);
    }
    if (bc->buf.ent != &bc->bufs[0]) {
        ppe_mp_free(bc->buf.ent);
    }
    ppe_mp_free(bc->ref.ent);
}

PPE_API void ppe_sbc_destroy(ppe_str_bunch restrict bc)
{
    if (bc) {
        ppe_sbc_clean(bc);
        ppe_mp_free(bc);
    }
}

PPE_API void ppe_sbc_reset(ppe_str_bunch restrict bc)
{
    ppe_uint i = 0;

    if (bc) {
        for (i = 0; i <= bc->buf.i; ++i) {
            bc->buf.ent[i]->pos = bc->buf.ent[i]->buf;
        }
        bc->buf.i = 0;
        bc->ref.i = 0;
    }
}

static ppe_bool ppe_sbc_augment_references(ppe_str_bunch restrict bc)
{
    ppe_string_st * nw = NULL;
    ppe_uint cap = 0;

    /* The new capacity is 1.5 times of the old one. */
    cap = (bc->ref.n + (bc->ref.n >> 1));

    nw = (ppe_string_st *) ppe_mp_calloc(cap, sizeof(ppe_string_st));
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return ppe_false;
    }

    if (bc->ref.ent) {
        memcpy(nw, bc->ref.ent, sizeof(ppe_string_st) * bc->ref.i);
        if (bc->ref.ent != &bc->strs[0]) {
            ppe_mp_free(bc->ref.ent);
        }
    }

    bc->ref.ent = nw;
    bc->ref.n = cap;
    return ppe_true;
}

static ppe_bool ppe_sbc_augment_buffer(ppe_sbc_buffer restrict buf, const ppe_ssize cap, const ppe_ssize sz)
{
    void * nw = NULL;
    ppe_ssize bytes = 0;

    bytes = (cap < sz) ? sz * 2 : cap;

    nw = ppe_mp_malloc(bytes);
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return ppe_false;
    }

    ppe_mp_free(buf->ptr);
    buf->ptr = nw;
    buf->pos = nw;
    buf->end = nw + bytes;
    return ppe_true;
}

static ppe_bool ppe_sbc_augment_buffers(ppe_str_bunch restrict bc, const ppe_ssize sz)
{
    ppe_sbc_buffer_st * nw = NULL;
    ppe_uint cap = 0;
    ppe_size rem = 0;

    /* The new capacity is 1.5 times of the old one. */
    cap = (bc->buf.n + (bc->buf.n >> 1));

    nw = (ppe_sbc_buffer_st *) ppe_mp_calloc(cap, sizeof(ppe_sbc_buffer_st));
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return ppe_false;
    }

    if (bc->buf.ent) {
        memcpy(nw, bc->buf.ent, sizeof(ppe_sbc_buffer_st) * bc->buf.n);
        if (bc->buf.ent != &bc->bufs[0]) {
            ppe_mp_free(bc->buf.ent);
        }
    }

    bc->buf.ent = nw;
    bc->buf.n = cap;
    return ppe_true;
}

PPE_API ppe_bool ppe_sbc_push_refer_to_cstr(ppe_str_bunch restrict bc, const char * restrict s, const ppe_ssize sz)
{
    /* Reference to an external C-style string. */

    assert(bc != NULL);
    assert(s != NULL && sz >= 0);

    if (bc->ref.i == bc->ref.n && ! ppe_sbc_augment_references(bc)) {
        return ppe_false;
    }

    bc->ref.ent[bc->ref.i].ptr = s;
    bc->ref.ent[bc->ref.i].sz = sz;

    bc->ref.i += 1;
    bc->total += sz;
    return ppe_true;
}

PPE_API ppe_bool ppe_sbc_push_refer_to(ppe_str_bunch restrict bc, const ppe_string restrict s)
{
    /* Reference to an external string. */

    assert(bc != NULL);
    assert(s != NULL);

    if (bc->ref.i == bc->ref.n && ! ppe_sbc_augment_references(bc)) {
        return ppe_false;
    }

    bc->ref.ent[bc->ref.i].ptr = s->buf;
    bc->ref.ent[bc->ref.i].sz = s->sz;

    bc->ref.i += 1;
    bc->total += sz;
    return ppe_true;
}

static ppe_bool ppe_sbc_push_copy_of_imp(ppe_str_bunch restrict bc, const char * restrict s, const ppe_ssize sz)
{
    ppe_sbc_buffer buf = NULL;
    ppe_bool out_of_buffer = ppe_false;

    if (bc->ref.i == bc->ref.n && ! ppe_sbc_augment_references(bc)) {
        return ppe_false;
    }

    /* INVARIABLE: bc->buf.i and buf is pointing to the current buffer to save string. */

    buf = &bc->buf.ent[bc->buf.i];
    out_of_buffer = (buf->end - buf->pos) < sz;
    if (buf->pos != buf->ptr && out_of_buffer) {
        /* The current buffer is holding something, */
        /* and there is not enough buffer, move to the next one. */
        if ((bc->buf.i + 1) == bc->buf.n && ! ppe_sbc_augment_buffers(bc, sz)) {
            return ppe_false;
        }
        bc->buf.i += 1;
        buf = &bc->buf.ent[bc->buf.i];
        out_of_buffer = (buf->end - buf->pos) < sz;
    }

    if (out_of_buffer && ! ppe_sbc_augment_buffer(buf, bc->buf.nwcap, sz)) {
        return ppe_false;
    }

    memcpy(buf->pos, s, sz);

    bc->ref.ent[bc->ref.i].ptr = buf->pos;
    bc->ref.ent[bc->ref.i].sz = sz;
    bc->ref.i += 1;

    buf->pos += sz;
    bc->total += sz;
    return ppe_true;
}

PPE_API ppe_bool ppe_sbc_push_copy_of_cstr(ppe_str_bunch restrict bc, const char * restrict s, const ppe_ssize sz)
{
    assert(bc != NULL);
    assert(s != NULL);
    return ppe_sbc_push_copy_of_imp(bc, s, sz);
}

PPE_API ppe_bool ppe_sbc_push_copy_of(ppe_str_bunch restrict bc, const ppe_string restrict s)
{
    assert(bc != NULL);
    assert(s != NULL);
    return ppe_sbc_push_copy_of_imp(bc, s->buf, s->sz);
}

PPE_API void ppe_sbc_pop_n(ppe_str_bunch restrict bc, const ppe_uint n)
{
    ppe_sbc_buffer buf = NULL;
    ppe_uint max = 0;
    ppe_uint i = 0;

    assert(bc != NULL);
    assert(n > 0);

    max = (bc->ref.i < n) ? bc->ref.i : n;

    buf = bc->buf.ent[bc->buf.i];
    for (i = 0; i < max; ++i) {
        bc->ref.i -= 1;
        bc->total -= bc->ref.ent[bc->ref.i].sz;

        if ((bc->ref.ent[bc->ref.i].ptr + bc->ref.ent[bc->ref.i].sz) == buf->pos) {
            buf->pos -= bc->ref.ent[bc->ref.i].sz;
            if (buf->pos == buf->ptr) {
                bc->buf.i -= 1;
                buf -= 1;
            }
        }
    } /* for */
}

static ppe_string ppe_sbc_join_by_cstr_imp(ppe_str_bunch restrict bc, const char * restrict d, const ppe_ssize dsz)
{
    ppe_string nw = NULL;
    ppe_ssize nwsz = 0;
    ppe_uint i = 0;

    nwsz = bc->ref.ent[0]->sz;
    if (dsz > 0) {
        for (i = 1; i < bc->ref.i; i += 1) {
            nwsz += dsz;
            nwsz += bc->ref.ent[i]->sz;
        }
    } else {
        for (i = 1; i < bc->ref.i; i += 1) {
            nwsz += bc->ref.ent[i]->sz;
        }
    }

    if (nwsz == 0) {
        /* All the source strings and the delimiter are empty. */
        return &str_empty_s;
    }

    nw = (ppe_string) ppe_mp_malloc(sizeof(ppe_string_st) + nwsz);
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }
    nw->buf = pos = ppe_str_buffer(nw);
    nw->sz = nwsz;

    memcpy(pos, bc->ref.ent[0].ptr, bc->ref.ent[0].sz);
    pos += bc->ref.ent[0].sz;

    if (dsz > 0) {
        for (i = 1; i < bc->ref.i; i += 1) {
            memcpy(pos, d, dsz);
            pos += dsz;

            memcpy(pos, bc->ref.ent[i].ptr, bc->ref.ent[i].sz);
            pos += bc->ref.ent[i].sz;
        }
    } else {
        for (i = 1; i < bc->ref.i; i += 1) {
            memcpy(pos, bc->ref.ent[i].ptr, bc->ref.ent[i].sz);
            pos += bc->ref.ent[i].sz;
        }
    }

    *pos = '\0';
    return nw;
}

PPE_API ppe_string ppe_sbc_join_by_cstr(const ppe_str_bunch restrict bc, const char * restrict d, const ppe_ssize dsz)
{
    assert(bc != NULL);
    assert(d != NULL && dsz >= 0);

    if (bc->ref.i == 0) {
        return &str_empty_s;
    }
    if (bc->ref.i == 1) {
        return ppe_cs_clone(bc->ref.ent[0].ptr, bc->ref.ent[0].sz);
    }
    return ppe_sbc_join_by_cstr_imp(bc, d, dsz);
}

PPE_API ppe_string ppe_sbc_join_by(const ppe_str_bunch restrict bc, const ppe_string restrict d)
{
    assert(bc != NULL);
    assert(d != NULL);

    if (bc->ref.i == 0) {
        return &str_empty_s;
    }
    if (bc->ref.i == 1) {
        return ppe_cs_clone(bc->ref.ent[0].ptr, bc->ref.ent[0].sz);
    }
    return ppe_sbc_join_by_cstr_imp(bc, d->buf, d->sz);
}

PPE_API ppe_string ppe_sbc_concat(const ppe_str_bunch restrict bc)
{
    assert(bc != NULL);

    if (bc->ref.i == 0) {
        return &str_empty_s;
    }
    if (bc->ref.i == 1) {
        return ppe_cs_clone(bc->ref.ent[0].ptr, bc->ref.ent[0].sz);
    }
    return ppe_sbc_join_by_cstr_imp(bc, "", 0);
}

PPE_API ppe_bool ppe_sbc_next_buffer(const ppe_str_bunch restrict bc, void ** restrict dat, const char ** restrict ptr, ppe_size * restrict sz)
{
    ppe_sbc_buffer buf = (ppe_sbc_buffer) dat;

    assert(bc != NULL);
    assert(dat != NULL);
    assert(ptr != NULL);
    assert(sz != NULL);

    if (! bc->buf.ent) {
        /* No any buffer exists. */
        return ppe_false;
    }

    if (! buf) {
        buf = &bc->buf.ent[0];
    } else {
        buf += 1;
        if (buf == &bc->buf.ent[bc->buf.n]) {
            /* No more buffers. */
            return ppe_false;
        }
    }

    if (buf->pos == buf->ptr) {
        /* No more data. */
        return ppe_false;
    }

    *ptr = buf->ptr;
    *sz = buf->pos - buf->ptr;
    *dat = (void *) buf;
    return ppe_true;
}

PPE_API ppe_bool ppe_sbc_reference(const ppe_str_bunch restrict bc, const ppe_uint idx, const char ** restrict s, ppe_ssize * restrict sz)
{
    assert(bc != NULL);
    assert(s != NULL);
    assert(sz != NULL);

    if (bc->ref.i <= idx) {
        ppe_err_set(PPE_ERR_NO_SUCH_ENTRY, NULL);
        return ppe_false;
    }
    *s = bc->ref.ent[idx].ptr;
    *sz = bc->ref.ent[idx].sz;
    return ppe_true;
}

PPE_API ppe_uint ppe_sbc_string_count(const ppe_str_bunch restrict bc)
{
    assert(bc != NULL);
    return bc->ref.i;
}

PPE_API ppe_uint ppe_sbc_buffer_count(const ppe_str_bunch restrict bc)
{
    assert(bc != NULL);
    return bc->buf.i;
}

PPE_API ppe_ssize ppe_sbc_total_size(const ppe_str_bunch restrict bc)
{
    assert(bc != NULL);
    return bc->total;
}

#ifdef __cplusplus
}
#endif

