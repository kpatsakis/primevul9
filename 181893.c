static inline struct capi20_appl *__get_capi_appl_by_nr(u16 applid)
{
	lockdep_assert_held(&capi_controller_lock);

	if (applid < 1 || applid - 1 >= CAPI_MAXAPPL)
		return NULL;

	return capi_applications[applid - 1];
}