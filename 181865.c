u16 capi20_get_version(u32 contr, struct capi_version *verp)
{
	struct capi_ctr *ctr;
	u16 ret;

	if (contr == 0) {
		*verp = driver_version;
		return CAPI_NOERROR;
	}

	mutex_lock(&capi_controller_lock);

	ctr = get_capi_ctr_by_nr(contr);
	if (ctr && ctr->state == CAPI_CTR_RUNNING) {
		memcpy(verp, &ctr->version, sizeof(capi_version));
		ret = CAPI_NOERROR;
	} else
		ret = CAPI_REGNOTINSTALLED;

	mutex_unlock(&capi_controller_lock);
	return ret;
}