u16 capi20_get_profile(u32 contr, struct capi_profile *profp)
{
	struct capi_ctr *ctr;
	u16 ret;

	if (contr == 0) {
		profp->ncontroller = ncontrollers;
		return CAPI_NOERROR;
	}

	mutex_lock(&capi_controller_lock);

	ctr = get_capi_ctr_by_nr(contr);
	if (ctr && ctr->state == CAPI_CTR_RUNNING) {
		memcpy(profp, &ctr->profile, sizeof(struct capi_profile));
		ret = CAPI_NOERROR;
	} else
		ret = CAPI_REGNOTINSTALLED;

	mutex_unlock(&capi_controller_lock);
	return ret;
}