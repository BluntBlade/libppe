#include <assert.h>
#include "ppe/string/finder.h"

#ifdef __cplusplus
extern "C"
{
#endif

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
    return ppe_sfd_create_for_cstr(s->buf, s->sz);
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
    ppe_sfd_reset_for_imp(fd, s->buf, s->sz);
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

static ppe_int ppe_sfd_find_n_imp(ppe_str_finder restrict fd, const char * s restrict, const ppe_ssize sz, const ppe_int n, const ppe_bool copy, ppe_str_bunch restrict bcc, ppe_str_bunch restrict bcs)
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
            if (copy) {
                ret = ppe_sbc_push_copy_of_cstr(bcc, begin, pos - begin);
            } else {
                ret = ppe_sbc_push_refer_to_cstr(bcc, begin, pos - begin);
            }
            if (! ret) {
                goto PPE_SFD_FIND_N_IMP_ERROR_HANDLING;
            }
            bccn += 1;
        }
        if (bcs) {
            if (copy) {
                ret = ppe_sbc_push_copy_of_cstr(bcs, pos, sz);
            } else {
                ret = ppe_sbc_push_refer_to_cstr(bcs, pos, sz);
            }
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
        ppe_sbc_pop_n(bcc, 1);
    }
    if (bcs && bcsn > i) {
        ppe_sbc_pop_n(bcs, 1);
    }
    return 0 - i;
}

PPE_API ppe_int ppe_sfd_find_n_for_cstr(ppe_str_finder restrict fd, const char * s restrict, const ppe_ssize sz, const ppe_int n, const ppe_bool copy, ppe_str_bunch restrict bcc, ppe_str_bunch restrict bcs)
{
    assert(fd != NULL);
    assert(s != NULL && sz > 0);
    assert(bcc != NULL);
    assert(bcs != NULL);
    if (sz == 0) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return ppe_false;
    }
    return ppe_sfd_find_n_imp(fd, s, sz, n, copy, bcc, bcs);
}

PPE_API ppe_int ppe_sfd_find_n_for(ppe_str_finder restrict fd, const ppe_string restrict s, const ppe_int n, const ppe_bool copy, ppe_str_bunch restrict bcc, ppe_str_bunch restrict bcs)
{
    assert(fd != NULL);
    assert(s != NULL);
    assert(bcc != NULL);
    assert(bcs != NULL);
    if (s->sz == 0) {
        ppe_err_set(PPE_ERR_INVALID_ARGUMENT, NULL);
        return ppe_false;
    }
    return ppe_sfd_find_n_imp(fd, s->buf, s->sz, n, copy, bcc, bcs);
}

PPE_API ppe_bool ppe_sfd_find_last_component(ppe_str_finder restrict fd, const ppe_bool copy, ppe_str_bunch restrict bcc)
{
    assert(fd != NULL);
    assert(bcc != NULL);
    if (copy) {
        return ppe_sbc_push_copy_of_cstr(bcc, fd->pos, fd->end - fd->pos);
    }
    return ppe_sbc_push_refer_to_cstr(bcc, fd->pos, fd->end - fd->pos);
}

#ifdef __cplusplus
}
#endif

