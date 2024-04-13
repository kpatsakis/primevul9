capi_ctr_put(struct capi_ctr *ctr)
{
	module_put(ctr->owner);
}