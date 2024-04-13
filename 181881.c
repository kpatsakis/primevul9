static void ctr_down(struct capi_ctr *ctr, int new_state)
{
	struct capi20_appl *ap;
	u16 applid;

	if (ctr->state == CAPI_CTR_DETECTED || ctr->state == CAPI_CTR_DETACHED)
		return;

	ctr->state = new_state;

	memset(ctr->manu, 0, sizeof(ctr->manu));
	memset(&ctr->version, 0, sizeof(ctr->version));
	memset(&ctr->profile, 0, sizeof(ctr->profile));
	memset(ctr->serial, 0, sizeof(ctr->serial));

	for (applid = 1; applid <= CAPI_MAXAPPL; applid++) {
		ap = __get_capi_appl_by_nr(applid);
		if (ap)
			capi_ctr_put(ctr);
	}
}