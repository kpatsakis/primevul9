static inline struct capi_ctr *get_capi_ctr_by_nr(u16 contr)
{
	if (contr < 1 || contr - 1 >= CAPI_MAXCONTR)
		return NULL;

	return capi_controller[contr - 1];
}