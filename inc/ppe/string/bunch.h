#ifndef PPE_STRING_BUNCH_H
#define PPE_STRING_BUNCH_H 1 

/* ==== Declaration : String Bunch ==== */

/* ---- Types ---- */

struct ppe_str_bunch_st;
typedef struct ppe_str_bunch_st * ppe_str_bunch;

/* ---- Functions ---- */

PPE_API extern ppe_str_bunch ppe_sbc_create(void);
PPE_API extern void ppe_sbc_destroy(ppe_str_bunch restrict bc);
PPE_API extern void ppe_sbc_reset(ppe_str_bunch restrict bc);

PPE_API extern ppe_bool ppe_sbc_push_refer_to_cstr(ppe_str_bunch restrict bc, const char * restrict s, const ppe_ssize sz);
PPE_API extern ppe_bool ppe_sbc_push_refer_to(ppe_str_bunch restrict bc, const ppe_string restrict s);

PPE_API extern ppe_bool ppe_sbc_push_copy_of_cstr(ppe_str_bunch restrict bc, const char * restrict s, const ppe_ssize sz);
PPE_API extern ppe_bool ppe_sbc_push_copy_of(ppe_str_bunch restrict bc, const ppe_string restrict s);

PPE_API extern void ppe_sbc_pop_n(ppe_str_bunch restrict bc, const ppe_uint n);

PPE_API extern ppe_string ppe_sbc_join_by_cstr(const ppe_str_bunch restrict bc, const char * restrict s, const ppe_ssize sz);
PPE_API extern ppe_string ppe_sbc_join_by(const ppe_str_bunch restrict bc, const ppe_string restrict s);

PPE_API extern ppe_string ppe_sbc_concat(const ppe_str_bunch restrict bc);

PPE_API extern ppe_bool ppe_sbc_next_buffer(const ppe_str_bunch restrict bc, void ** restrict dat, const char ** restrict ptr, ppe_size * restrict sz);

PPE_API extern ppe_bool ppe_sbc_reference(const ppe_str_bunch restrict bc, const ppe_uint idx, const char ** restrict s, ppe_ssize * restrict sz);
PPE_API extern ppe_uint ppe_sbc_string_count(const ppe_str_bunch restrict bc);
PPE_API extern ppe_ssize ppe_sbc_total_size(const ppe_str_bunch restrict bc);

#endif /* PPE_STRING_BUNCH_H */
