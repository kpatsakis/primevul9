capi_ctr_get(struct capi_ctr *ctr)
{
	if (!try_module_get(ctr->owner))
		return NULL;
	return ctr;
}