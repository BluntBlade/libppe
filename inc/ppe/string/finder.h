#ifndef PPE_STRING_FINDER_H
#define PPE_STRING_FINDER_H 1 

/* ==== Declaration : String Finder ==== */

/* ---- Types ---- */

struct ppe_str_finder_st;
typedef struct ppe_str_finder_st * ppe_str_finder;

/* ---- Functions ---- */

PPE_API extern ppe_str_finder ppe_sfd_create_for_cstr(const char * restrict s, const ppe_ssize sz);
PPE_API extern ppe_str_finder ppe_sfd_create_for(const ppe_string restrict s);
PPE_API extern void ppe_sfd_destroy(ppe_str_finder restrict fd);

PPE_API extern ppe_bool ppe_sfd_reset_for_cstr(ppe_str_finder restrict fd, const char * restrict s, const ppe_ssize sz);
PPE_API extern ppe_bool ppe_sfd_reset_for(ppe_str_finder restrict fd, const ppe_string restrict s);

PPE_API extern ppe_int ppe_sfd_find_n_for_cstr(ppe_str_finder restrict fd, const char * s restrict, const ppe_ssize sz, const ppe_int n, const ppe_bool copy, ppe_str_bunch restrict bcc, ppe_str_bunch restrict bcs);
PPE_API extern ppe_int ppe_sfd_find_n_for(ppe_str_finder restrict fd, const ppe_string restrict s, const ppe_int n, const ppe_bool copy, pe_str_bunch restrict bcc, ppe_str_bunch restrict bcs);

PPE_API extern ppe_bool ppe_sfd_find_last_component(ppe_str_finder restrict fd, const ppe_bool copy, ppe_str_bunch restrict bcc);

#endif /* PPE_STRING_FINDER_H */

