static void release_appl(struct capi_ctr *ctr, u16 applid)
{
	DBG("applid %#x", applid);

	ctr->release_appl(ctr, applid);
	capi_ctr_put(ctr);
}