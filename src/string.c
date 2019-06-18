#include "ppe/mempool.h"
#include "ppe/string.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ==== Declaration : String ==== */

/* ---- Types ---- */

typedef struct PPE_STRING
{
    ppe_ssize sz;
    const char * ptr;
} ppe_string_st;

/* ---- Functions ---- */

/* -- Internals -- */

static inline ppe_ssize ppe_cs_detect_size(const char * restrict s, const ppe_ssize sz)
{
    return (sz == PPE_STR_DETECT_SIZE) ? strlen(s) : sz;
}

static inline char * ppe_str_buffer(ppe_string restrict s)
{
    return (((char *)&s->ptr) + sizeof(s->ptr));
}

/* ==== Definitions : String ==== */

/* ---- Preset values ---- */

static const char * cs_empty_s = "";
static ppe_string_st str_empty_s = { 0, cs_empty_s };
static ppe_string str_empty_array_s[2] = { &str_empty_s, NULL };

/* ---- Functions ---- */

/* -- Preset values -- */

PPE_API const ppe_string ppe_str_get_empty(void)
{
    return &str_empty_s;
}

/* -- Create & Destroy -- */

/* _cs_ series */

PPE_API ppe_string ppe_cs_clone(const ppe_char * restrict s, const ppe_ssize sz)
{
    ppe_string nw = NULL;

    assert(s != NULL);

    if (s == str_empty_s.ptr) {
        return &str_empty_s;
    }
    sz = ppe_cs_detect_size(s, sz);

    if (sz == 0) {
        /* Check if the source string is empty. */
        return &str_empty_s;
    }

    nw = (ppe_string) ppe_mp_malloc(sizeof(ppe_string_st) + sz + 1);
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    nw->ptr = ppe_str_buffer(nw);
    memcpy(nw->ptr, s, sz);
    nw->ptr[sz] = '\0';
    nw->sz = sz;
    return nw;
}

PPE_API ppe_string ppe_cs_vsprintf(const char * restrict fmt, va_list args)
{
    /* TODO: Implementation */
    return NULL;
}

/* _str_ series */

PPE_API ppe_string ppe_str_clone(const ppe_string restrict s)
{
    assert(s != NULL);
    return ppe_cs_clone(s->ptr, s->sz);
}

PPE_API void ppe_str_destroy(ppe_string restrict s)
{
    if (s && s != &str_empty_s) {
        ppe_mp_free(s);
    }
}

/* -- Property -- */

PPE_API const char * ppe_str_cstr(ppe_string restrict s)
{
    assert(s != NULL);
    return s->ptr;
}

PPE_API ppe_ssize ppe_str_size(ppe_string restrict s)
{
    assert(s != NULL);
    return s->sz;
}

/* -- Comparison -- */

PPE_API ppe_bool ppe_str_equals(const ppe_string restrict s1, const ppe_string restrict s2)
{
    assert(s1 != NULL);
    assert(s2 != NULL);
    return strcmp(s1->ptr, s2->ptr) == 0;
}

PPE_API ppe_bool ppe_str_less_than(const ppe_string restrict s1, const ppe_string restrict s2)
{
    assert(s1 != NULL);
    assert(s2 != NULL);
    return strcmp(s1->ptr, s2->ptr) < 0;
}

PPE_API ppe_bool ppe_str_greater_than(const ppe_string restrict s1, const ppe_string restrict s2)
{
    assert(s1 != NULL);
    assert(s2 != NULL);
    return strcmp(s1->ptr, s2->ptr) > 0;
}

/* ==== Definition : String Bunch ==== */

/* ---- Types ---- */

struct PPE_SBC_BUFFER;

typedef struct PPE_SBC_BUFFER
{
    struct PPE_SBC_BUFFER * prev;
    struct PPE_SBC_BUFFER * next;
    ppe_ssize rem;
    ppe_ssize cap;
} ppe_sbc_buffer_st, *ppe_sbc_buffer;

typedef struct PPE_STR_BUNCH
{
    struct {
        ppe_string_st * ent;
        ppe_uint i;             /* The count of used text snippets. */
        ppe_uint n;             /* The total amount of text snippets. */
    } ref;

    struct {
        ppe_sbc_buffer_st * head;
        ppe_sbc_buffer_st * last;
        ppe_sbc_buffer_st * curr;
        ppe_ssize nwcap;         /* The default capacity of new buffer, include the size of the ppe_sbc_buffer_st structure. */
    } buf;

    ppe_ssize total;             /* The total size of referenced and buffered strings in bytes. */
} ppe_str_bunch_st;

/* ---- Functions ---- */

#define PPE_SBC_BUFFER_SIZE 4096

static inline void ppe_sbc_init(ppe_str_bunch restrict bc)
{
    memset(bc, 0, sizeof(ppe_str_bunch_st));
    bc->buf.nwcap = PPE_SBC_BUFFER_SIZE;
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
    ppe_sbc_buffer_st * curr = NULL;
    ppe_sbc_buffer_st * next = NULL;

    curr = bc->buf.head;
    while (curr) {
        next = curr->next;
        ppe_mp_free(curr);
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

static char * ppe_sbc_buffer_position(const ppe_sbc_buffer restrict b)
{
    return (char *)b + sizeof(ppb_sbc_buffer_st) + (b->cap - b->rem);
}

PPE_API void ppe_sbc_reset(ppe_str_bunch restrict bc)
{
    ppe_sbc_buffer_st * curr = NULL;

    if (bc) {
        curr = bc->buf.head;
        while (curr) {
            curr->rem = curr->cap;
            curr = curr->next;
        }
        bc->buf.curr = bc->buf.head;
        bc->ref.i = 0;
    }
}

static ppe_bool ppe_sbc_augment_references(ppe_str_bunch restrict bc)
{
    ppe_string_st * nw = NULL;
    ppe_uint cap = 0;

    cap = (bc->ref.n > 0) ? (bc->ref.n + (bc->ref.n >> 1)) : 16; /* The new capacity is 1.5 times of the old one. */

    nw = (ppe_string_st *) ppe_mp_calloc(cap, sizeof(ppe_string_st));
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return ppe_false;
    }

    if (bc->ref.i > 0) {
        memcpy(nw, bc->ref.ent, bc->ref.i * sizeof(ppe_string_st));
        ppe_mp_free(bc->ref.ent);
    }

    bc->ref.ent = nw;
    bc->ref.n = cap;
    return ppe_true;
}

static ppe_bool ppe_sbc_augment_buffers(ppe_str_bunch restrict bc, const ppe_ssize sz)
{
    ppe_sbc_buffer_st * nw = NULL;
    ppe_ssize bytes = 0;

    if (bc->buf.curr != bc->buf.tail) {
        bc->buf.curr = bc->curr.next;
        return ppe_true;
    }

    if (sizeof(ppe_sbc_buffer_st) + sz < bc->buf.nwcap) {
        bytes = bc->buf.nwcap;
    } else {
        bytes = sizeof(ppe_sbc_buffer_st) + sz;
    }

    nw = (ppe_sbc_buffer_st *) ppe_mp_malloc(bytes);
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return ppe_false;
    }
    nw->rem = nw->cap = bytes - sizeof(ppe_sbc_buffer_st);
    nw->prev = NULL;
    nw->next = NULL;

    if (! bc->buf.head) {
        bc->buf.head = nw;
    } else {
        nw->prev = bc->buf.last;
        bc->buf.last->next = nw;
    }
    bc->buf.last = nw;
    bc->buf.curr = nw;

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

    bc->ref.ent[bc->ref.i].ptr = s->ptr;
    bc->ref.ent[bc->ref.i].sz = s->sz;

    bc->ref.i += 1;
    bc->total += sz;
    return ppe_true;
}

static ppe_bool ppe_sbc_push_copy_of_imp(ppe_str_bunch restrict bc, const char * restrict s, const ppe_ssize sz)
{
    if (bc->ref.i == bc->ref.n && ! ppe_sbc_augment_references(bc)) {
        return ppe_false;
    }
    if (bc->buf.curr->rem < sz && ! ppe_sbc_augment_buffers(bc, sz)) {
        return ppe_false;
    }

    bc->ref.ent[bc->ref.i].ptr = ppe_sbc_buffer_position(bc->buf.last);
    bc->ref.ent[bc->ref.i].sz = sz;
    memcpy(bc->ref.ent[bc->ref.i].ptr, s, sz);

    bc->ref.i += 1;
    bc->buf.last->rem -= sz;
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
    return ppe_sbc_push_copy_of_imp(bc, s->ptr, s->sz);
}

PPE_API void ppe_sbc_pop(ppe_str_bunch restrict bc)
{
    ppe_sbc_buffer curr = NULL;
    const char * end = NULL;

    assert(bc != NULL);

    if (bc->ref.i == 0) {
        return;
    }

    curr = (bc->buf.curr->rem == bc->buf.curr->cap) ? bc->buf.curr->prev : bc->buf.curr;

    end = bc->ref.ent[bc->ref.i - 1].ptr + bc->ref.ent[bc->ref.i - 1].sz;
    if (ppe_sbc_buffer_position(curr) == end) {
        curr->rem += bc->ref.ent[bc->ref.i - 1].sz;

        if (bc->buf.curr->prev == curr) {
            bc->buf.curr = curr;
        }
    }

    bc->total -= bc->ref.ent[bc->ref.i - 1].sz;
    bc->ref.i -= 1;
}

static ppe_string ppe_sbc_join_by_cstr_imp(ppe_str_bunch restrict bc, const char * restrict d, const ppe_ssize dsz)
{
    ppe_string nw = NULL;
    ppe_ssize nwsz = 0;
    ppe_uint i = 0;

    if (bc->ref.i == 0) {
        return &str_empty_s;
    }
    if (bc->ref.i == 1) {
        return ppe_cs_clone(bc->ref.ent[0].ptr, bc->ref.ent[0].sz);
    }

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

    nw = (ppe_string) ppe_mp_malloc(sizeof(ppe_string_st) + nwsz + 1);
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }
    pos = ppe_str_buffer(nw);

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
    nw->ptr = ppe_str_buffer(nw);
    nw->sz = nwsz;
    return nw;
}

PPE_API ppe_string ppe_sbc_join_by_cstr(const ppe_str_bunch restrict bc, const char * restrict d, const ppe_ssize dsz)
{
    assert(bc != NULL);
    assert(d != NULL && dsz >= 0);
    return ppe_sbc_join_by_cstr_imp(bc, d, dsz);
}

PPE_API ppe_string ppe_sbc_join_by(const ppe_str_bunch restrict bc, const ppe_string restrict d)
{
    assert(bc != NULL);
    assert(d != NULL);
    return ppe_sbc_join_by_cstr_imp(bc, d->ptr, d->sz);
}

PPE_API ppe_string ppe_sbc_concat(const ppe_str_bunch restrict bc)
{
    assert(bc != NULL);
    return ppe_sbc_join_by_cstr_imp(bc, "", 0);
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

PPE_API ppe_uint ppe_sbc_count(const ppe_str_bunch restrict bc)
{
    assert(bc != NULL);
    return bc->ref.i;
}

PPE_API ppe_ssize ppe_sbc_total_size(const ppe_str_bunch restrict bc)
{
    assert(bc != NULL);
    return bc->total;
}

/* ==== Definition : String Finder ==== */

/* ---- Types ---- */

typedef struct PPE_STR_FINDER
{
    const char * pos;
    const char * end;
} ppe_str_finder_st;

/* ---- Functions ---- */

static inline void ppe_sfd_reset_for_imp(ppe_str_finder restrict fd, const char * restrict s, const ppe_ssize sz)
{
    if (sz == 0) {
        fd->pos = str_empty_s.ptr;
        fd->end = str_empty_s.ptr;
    } else {
        fd->pos = s;
        fd->end = s + sz;
    }
}

PPE_API ppe_str_finder ppe_sfd_create_for_cstr(const char * restrict s, const ppe_ssize sz)
{
    ppe_str_finder nw = NULL;

    assert(s != NULL && sz >= 0);

    nw = ppe_mp_malloc(sizeof(ppe_str_finder_st));
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }
    ppe_sfd_reset_for_imp(nw, s, sz);
    return nw;
}

PPE_API ppe_str_finder ppe_sfd_create_for(const ppe_string restrict s)
{
    assert(s != NULL);
    return ppe_sfd_create_for_cstr(s->ptr, s->sz);
}

PPE_API void ppe_sfd_destroy(ppe_str_finder restrict fd)
{
    if (fd) {
        ppe_mp_free(fd);
    }
}

PPE_API ppe_bool ppe_sfd_reset_for_cstr(ppe_str_finder restrict fd, const char * restrict s, const ppe_ssize sz)
{
    assert(s != NULL && sz >= 0);
    ppe_sfd_reset_for_imp(fd, s, sz);
    return ppe_true;
}

PPE_API ppe_bool ppe_sfd_reset_for(ppe_str_finder restrict fd, const ppe_string restrict s)
{
    assert(s != NULL);
    ppe_sfd_reset_for_imp(fd, s->ptr, s->sz);
    return ppe_true;
}

static const char * ppe_sfd_find_imp(ppe_str_finder restrict fd, const char * restrict s, const ppe_ssize sz)
{
    const char * pos = NULL;
    ppe_ssize rem = 0;

    assert(fd->pos <= fd->end);

    rem = fd->end - fd->pos;
    if (rem < sz) {
        /* The part to search of the string is shorter than the target substring. */
        return NULL;
    }

    /* If the search failed, don't consume any characters and leave them for the next search of other substrings. */

    if (sz == 1) {
        if ((pos = memchr(fd->pos, s[0], rem))) {
            fd->pos = pos + sz; /* Stop at the start point of the next search. */
            return pos;
        }
        return NULL;
    }

    /* Detect the substring at every char. */
    /* Case:
     * -------ABC
     * --AB
     */
    for (pos = fd->pos; sz <= rem; ++pos, --rem) {
        pos = memchr(pos, s[0], rem);
        if (! pos) {
            return NULL;
        }

        if (memcmp(pos, s, sz) == 0) {
            fd->pos = pos + sz; /* Stop at the start point of the next search. */
            return pos;
        }
    } /* while */

    return NULL;
}

static ppe_int ppe_sfd_find_n_imp(ppe_str_finder restrict fd, const char * s restrict, const ppe_ssize sz, const ppe_int n, ppe_str_bunch restrict bcc, ppe_str_bunch restrict bcs)
{
    const char * begin = NULL;
    const char * pos = NULL;
    ppe_int max = 32;
    ppe_int i = 0;
    ppe_int bccn = 0;
    ppe_int bcsn = 0;

    if (0 < n && n < max) {
        max = n;
    }
   
    for (i = 0; i < max; ++i) {
        if (bcc) {
            assert(i == bccn);
        }
        if (bcs) {
            assert(i == bcsn);
        }

        begin = fd->pos;

        pos = ppe_sfd_find_imp(fd, s, sz);
        if (! pos) {
            break;
        }

        if (bcc) {
            ret = ppe_sbc_push_refer_to_cstr(bcc, begin, pos - begin);
            if (! ret) {
                goto PPE_SFD_FIND_N_IMP_ERROR_HANDLING;
            }
            bccn += 1;
        }
        if (bcs) {
            ret = ppe_sbc_push_refer_to_cstr(bcs, pos, sz);
            if (! ret) {
                goto PPE_SFD_FIND_N_IMP_ERROR_HANDLING;
            }
            bcsn += 1;
        }
    } /* for */
    return i;

PPE_SFD_FIND_N_IMP_ERROR_HANDLING:
    fd->pos = begin; /* Resume the start point of the next search. */

    if (bcc && bccn > i) {
        ppe_sbc_pop(bcc);
    }
    if (bcs && bcsn > i) {
        ppe_sbc_pop(bcs);
    }
    return 0 - i;
}

PPE_API ppe_int ppe_sfd_find_n_for_cstr(ppe_str_finder restrict fd, const char * s restrict, const ppe_ssize sz, const ppe_int n, ppe_str_bunch restrict bcc, ppe_str_bunch restrict bcs)
{
    assert(fd != NULL);
    assert(s != NULL && sz > 0);
    assert(bcc != NULL);
    assert(bcs != NULL);
    if (sz == 0) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return ppe_false;
    }
    return ppe_sfd_find_n_imp(fd, s, sz, n, bcc, bcs);
}

PPE_API ppe_int ppe_sfd_find_n_for(ppe_str_finder restrict fd, const ppe_string restrict s, const ppe_int n, ppe_str_bunch restrict bcc, ppe_str_bunch restrict bcs)
{
    assert(fd != NULL);
    assert(s != NULL);
    assert(bcc != NULL);
    assert(bcs != NULL);
    if (s->sz == 0) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return ppe_false;
    }
    return ppe_sfd_find_n_imp(fd, s->ptr, s->sz, n, bcc, bcs);
}

PPE_API ppe_bool ppe_sfd_refer_last_component(ppe_str_finder restrict fd, ppe_str_bunch restrict bcc)
{
    assert(fd != NULL);
    assert(bcc != NULL);
    return ppe_sbc_push_refer_to_cstr(bcc, fd->pos, fd->end - fd->pos);
}

/* ==== Definitions : String ==== */

/* ---- Functions Defered to Implement ---- */

/* -- cs module -- */

/* split */

static ppe_str_bunch ppe_cs_split_imp(const char * restrict d, const ppe_ssize dsz, const char * restrict s, const ppe_ssize sz, const ppe_int n)
{
    ppe_str_finder_st fd;
    ppe_str_bunch bcc = NULL;
    ppe_int ret = 0;

    bcc = ppe_sbc_create();
    if (! bcc) {
        return NULL;
    }

    ppe_sfd_reset_for_cstr(&fd, s, sz);

    ret = ppe_sfd_find_n_imp(&fd, d, dsz, n, bcc, NULL); 
    if (ret < 0) {
        ppe_sbc_destroy(bcc);
        return NULL;
    }
    if (! ppe_sfd_refer_last_component(&fd)) {
        ppe_sbc_destroy(bcc);
        return NULL;
    }
    return bcc;
}

PPE_API ppe_str_bunch ppe_cs_split_cstr(const char * restrict d, const ppe_ssize dsz, const char * restrict s, const ppe_ssize sz, const ppe_int n)
{
    assert(d != NULL && dsz > 0);
    assert(s != NULL);
    return ppe_cs_split_imp(d, dsz, s, sz, n);
}

PPE_API ppe_str_bunch ppe_cs_split(const char * restrict d, const ppe_ssize dsz, const ppe_string restrict s, const ppe_int n)
{
    assert(d != NULL && dsz > 0);
    assert(s != NULL);
    return ppe_cs_split_imp(d, dsz, s->ptr, s->sz, n);
}

/* join */

static ppe_string ppe_cs_join_2_imp(const char * restrict d, ppe_ssize dsz, const char * restrict s1, const ppe_ssize sz1, const char * restrict s2, const ppe_ssize sz2)
{
    ppe_string nw = NULL;
    ppe_ssize rem = PPE_STR_MAX_SIZE;

    if ((rem -= sz1) < 0) {
        ppe_err_set(PPE_ERR_OVERFLOW_UPPER_BOUND, NULL);
        return NULL;
    }
    if ((rem -= sz2) < 0) {
        ppe_err_set(PPE_ERR_OVERFLOW_UPPER_BOUND, NULL);
        return NULL;
    }

    if (rem == PPE_STR_MAX_SIZE) {
        /* Both source strings are empty. */
        return ppe_cs_clone(d, dsz);
    }

    if ((rem -= dsz) < 0) {
        ppe_err_set(PPE_ERR_OVERFLOW_UPPER_BOUND, NULL);
        return NULL;
    }

    nw = (ppe_string) ppe_mp_malloc(sizeof(ppe_string_st) + (PPE_STR_MAX_SIZE - rem) + 1);
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }
    nw->ptr = ppe_str_buffer(nw);
    nw->sz = 0;

    if (sz1 > 0) {
        memcpy(nw->ptr + nw->sz, s1, sz1);
        nw->sz += sz1;
    }

    memcpy(nw->ptr + nw->sz, d, dsz);
    nw->sz += dsz;

    if (sz2 > 0) {
        memcpy(nw->ptr + nw->sz, s2, sz2);
        nw->sz += sz2;
    }

    nw->ptr[nw->sz] = '\0';
    return nw;
}

static ppe_string ppe_cs_concat_2_imp(const char * restrict s1, const ppe_ssize sz1, const char * restrict s2, const ppe_ssize sz2)
{
    ppe_string nw = NULL;
    ppe_ssize rem = PPE_STR_MAX_SIZE;

    if ((rem -= sz1) < 0) {
        ppe_err_set(PPE_ERR_OVERFLOW_UPPER_BOUND, NULL);
        return NULL;
    }
    if ((rem -= sz2) < 0) {
        ppe_err_set(PPE_ERR_OVERFLOW_UPPER_BOUND, NULL);
        return NULL;
    }

    if (rem == PPE_STR_MAX_SIZE) {
        /* Both source strings are empty. */
        return &str_empty_s;
    }

    if (PPE_STR_MAX_SIZE - rem == sz1) {
        return ppe_cs_clone(s1, sz1);
    }
    if (PPE_STR_MAX_SIZE - rem == sz2) {
        return ppe_cs_clone(s2, sz2);
    }

    nw = (ppe_string) ppe_mp_malloc(sizeof(ppe_string_st) + (PPE_STR_MAX_SIZE - rem) + 1);
    if (! nw) {
        ppe_err_set(PPE_ERR_OUT_OF_MEMORY, NULL);
        return NULL;
    }
    nw->ptr = ppe_str_buffer(nw);
    nw->sz = 0;

    memcpy(nw->ptr, s1, sz1);
    nw->sz += sz1;
    memcpy(nw->ptr + nw->sz, s2, sz2);
    nw->sz += sz2;
    nw->ptr[nw->sz] = '\0';
    return nw;
}

static ppe_string ppe_cs_join_cstr_imp(const char * restrict d, const ppe_ssize dsz, const char * restrict s1, const ppe_ssize sz1, const char * restrict s2, const ppe_ssize sz2, va_list args)
{
    va_list cp;
    ppe_str_bunch bc;
    ppe_string nw = NULL;
    const char * s = NULL;
    ppe_ssize sz = 0;
    ppe_uint n = 0;

    /* -- Check arguments. -- */
    va_copy(cp, args);

    s = va_arg(cp, const char *);
    if (s == PPE_STR_ARG_END) {
        /* Only two strings to join. */
        va_end(cp);
        if (dsz == 0) {
            return ppe_cs_concat_2_imp(s1, sz1, s2, sz2);
        }
        return ppe_cs_join_2_imp(d, dsz, s1, sz1, s2, sz2);
    }

    /* -- Prepare operands. -- */
    ppe_sbc_init(&bc);

    if (! ppe_sbc_push_refer_to_cstr(bc, s1, sz1)) {
        goto PPE_CS_JOIN_CSTR_ERROR_HANDLING;
    }
    if (! ppe_sbc_push_refer_to_cstr(bc, s2, sz2)) {
        goto PPE_CS_JOIN_CSTR_ERROR_HANDLING;
    }

    do {
        sz = va_arg(cp, ppe_ssize);
        if (sz < 0) {
            ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
            goto PPE_CS_JOIN_CSTR_ERROR_HANDLING;
        }
        if (! ppe_sbc_push_refer_to_cstr(bc, s, sz)) {
            goto PPE_CS_JOIN_CSTR_ERROR_HANDLING;
        }

        s = va_arg(cp, const char *);
    } while (s != PPE_STR_ARG_END);

    nw = ppe_sbc_join_by_cstr_imp(bc, d, dsz);

PPE_CS_JOIN_CSTR_ERROR_HANDLING:
    ppe_sbc_clean(&bc);
    va_end(cp);
    return nw;
}

PPE_API ppe_string ppe_cs_join_cstr(const char * restrict d, const ppe_ssize dsz, const char * restrict s1, const ppe_ssize sz1, const char * restrict s2, const ppe_ssize sz2, ...)
{
    va_list ap;
    ppe_string nw = NULL;

    assert(d != NULL && dsz >= 0);
    assert(s1 != NULL && sz1 >= 0);
    assert(s2 != NULL && sz2 >= 0);

    va_start(ap, sz2);
    nw = ppe_cs_join_cstr_imp(d, dsz, s1, sz1, s2, sz2, ap);
    va_end(ap);
    return nw;
}

static ppe_string ppe_cs_join_imp(const char * restrict d, const ppe_ssize dsz, const ppe_string restrict s1, const ppe_string restrict s2, va_list args)
{
    va_list cp;
    ppe_str_bunch bc;
    ppe_string nw = NULL;
    ppe_string s = NULL;
    ppe_uint n = 0;

    /* -- Check arguments. -- */
    va_copy(cp, args);

    s = va_arg(cp, ppe_string);
    if (s == PPE_STR_ARG_END) {
        /* Only two strings to join. */
        va_end(cp);
        if (dsz == 0) {
            return ppe_cs_concat_2_imp(s1->ptr, s1->sz, s2->ptr, s2->sz);
        }
        return ppe_cs_join_2_imp(d, dsz, s1->ptr, s1->sz, s2->ptr, s2->sz);
    }

    /* -- Prepare operands. -- */
    ppe_sbc_init(&bc);

    if (! ppe_sbc_push_refer_to(bc, s1)) {
        goto PPE_CS_JOIN_ERROR_HANDLING;
    }
    if (! ppe_sbc_push_refer_to(bc, s2)) {
        goto PPE_CS_JOIN_ERROR_HANDLING;
    }

    do {
        if (! ppe_sbc_push_refer_to(bc, s)) {
            goto PPE_CS_JOIN_ERROR_HANDLING;
        }

        s = va_arg(cp, ppe_string);
    } while (s != PPE_STR_ARG_END);

    nw = ppe_sbc_join_by_cstr_imp(bc, d, dsz);

PPE_CS_JOIN_ERROR_HANDLING:
    ppe_sbc_clean(&bc);
    va_end(cp);
    return nw;
}

PPE_API ppe_string ppe_cs_join(const char * restrict d, const ppe_ssize dsz, const ppe_string restrict s1, const ppe_string restrict s2, ...)
{
    va_list ap;
    ppe_string nw = NULL;

    assert(d != NULL && dsz >= 0);
    assert(s1 != NULL);
    assert(s2 != NULL);

    va_start(ap, s2);
    nw = ppe_cs_join_imp(d, dsz, s1, s2, ap);
    va_end(ap);
    return nw;
}

PPE_API ppe_string ppe_cs_join_cstr_2(const char * restrict d, const ppe_ssize dsz, const char * restrict s1, const ppe_ssize sz1, const char * restrict s2, const ppe_ssize sz2)
{
    assert(d != NULL && dsz >= 0);
    assert(s1 != NULL);
    assert(s2 != NULL);
    if (dsz == 0) {
        return ppe_cs_concat_2_imp(s1, sz1, s2, sz2);
    }
    return ppe_cs_join_2_imp(d, dsz, s1, sz1, s2, sz2);
}

PPE_API ppe_string ppe_cs_join_2(const char * restrict d, const ppe_ssize dsz, const ppe_string restrict s1, const ppe_string restrict s2)
{
    assert(d != NULL && dsz >= 0);
    assert(s1 != NULL);
    assert(s2 != NULL);
    if (dsz == 0) {
        return ppe_cs_concat_2_imp(s1->ptr, s1->sz, s2->ptr, s2->sz);
    }
    return ppe_cs_join_2_imp(d, dsz, s1->ptr, s1->sz, s2->ptr, s2->sz);
}

/* concat */

PPE_API ppe_string ppe_cs_concat_cstr(const char * restrict s1, const ppe_ssize sz1, const char * restrict s2, const ppe_ssize sz2, ...)
{
    va_list ap;
    ppe_string nw = NULL;

    assert(s1 != NULL && sz1 >= 0);
    assert(s2 != NULL && sz2 >= 0);

    va_start(ap, sz2);
    nw = ppe_cs_join_cstr_imp("", 0, s1, sz1, s2, sz2, ap);
    va_end(ap);
    return nw;
}

PPE_API ppe_string ppe_cs_concat_cstr_2(const char * restrict s1, const ppe_ssize sz1, const char * restrict s2, const ppe_ssize sz2)
{
    assert(s1 != NULL && sz1 >= 0);
    assert(s2 != NULL && sz2 >= 0);
    return ppe_cs_concat_2_imp(s1, sz1, s2, sz2);
}

/* -- str module -- */

/* join */

PPE_API ppe_string ppe_str_join_cstr(const ppe_string restrict d, const char * restrict s1, const ppe_ssize sz1, const char * restrict s2, const ppe_ssize sz2, ...)
{
    va_list ap;
    ppe_string nw = NULL;

    assert(d != NULL);
    assert(s1 != NULL && sz1 >= 0);
    assert(s2 != NULL && sz2 >= 0);

    va_start(ap, sz2);
    nw = ppe_cs_join_cstr_imp(d->ptr, d->sz, s1, sz1, s2, sz2, ap);
    va_end(ap);
    return nw;
}

PPE_API ppe_string ppe_str_join(const ppe_string restrict d, const ppe_string restrict s1, const ppe_string restrict s2, ...)
{
    va_list ap;
    ppe_string nw = NULL;

    assert(d != NULL);
    assert(s1 != NULL && sz1 >= 0);
    assert(s2 != NULL && sz2 >= 0);

    va_start(ap, s2);
    nw = ppe_cs_join_imp(d->ptr, d->sz, s1, s2, ap);
    va_end(ap);
    return nw;
}

PPE_API ppe_string ppe_str_join_cstr_2(const ppe_string restrict d, const char * restrict s1, const ppe_ssize sz1, const char * restrict s2, const ppe_ssize sz2)
{
    assert(d != NULL);
    assert(s1 != NULL && sz1 >= 0);
    assert(s2 != NULL && sz2 >= 0);
    if (d->sz == 0) {
        return ppe_cs_concat_2_imp(s1, sz1, s2, sz2);
    }
    return ppe_cs_join_2_imp(d->ptr, d->sz, s1, sz1, s2, sz2);
}

PPE_API ppe_string ppe_str_join_2(const ppe_string restrict d, const ppe_string restrict s1, const ppe_string restrict s2)
{
    assert(d != NULL);
    assert(s1 != NULL);
    assert(s2 != NULL);
    if (d->sz == 0) {
        return ppe_cs_concat_2_imp(s1->ptr, s1->sz, s2->ptr, s2->sz);
    }
    return ppe_cs_join_2_imp(d->ptr, d->sz, s1->ptr, s1->sz, s2->ptr, s2->sz);
}

/* concat */

PPE_API ppe_string ppe_str_concat(const ppe_string restrict s1, const ppe_string restrict s2, ...)
{
    va_list ap;
    ppe_string nw = NULL;

    assert(s1 != NULL);
    assert(s2 != NULL);

    va_start(ap, s2);
    nw = ppe_cs_join_imp("", 0, s1, s2, ap);
    va_end(ap);
    return nw;
}

PPE_API ppe_string ppe_str_concat_2(const ppe_string restrict s1, const ppe_string restrict s2)
{
    assert(s1 != NULL);
    assert(s2 != NULL);
    return ppe_cs_concat_2_imp(s1->ptr, s1->sz, s2->ptr, s2->sz);
}

/* split */

PPE_API ppe_str_bunch ppe_str_split_cstr(const ppe_string restrict d, const char * restrict s, const ppe_ssize sz, const ppe_int n)
{
    assert(d != NULL && d->sz > 0);
    assert(s != NULL);
    return ppe_cs_split_imp(d->ptr, d->sz, s, sz, n);
}

PPE_API ppe_str_bunch ppe_str_split(const ppe_string restrict d, const ppe_string restrict s, const ppe_int n)
{
    assert(d != NULL && d->sz > 0);
    assert(s != NULL);
    return ppe_cs_split_imp(d->ptr, d->sz, s->ptr, s->sz, n);
}

#ifdef __cplusplus
}
#endif

