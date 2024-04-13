static inline struct capi20_appl *get_capi_appl_by_nr(u16 applid)
{
	if (applid < 1 || applid - 1 >= CAPI_MAXAPPL)
		return NULL;

	return rcu_dereference(capi_applications[applid - 1]);
}