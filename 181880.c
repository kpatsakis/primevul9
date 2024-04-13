static void notify_up(u32 contr)
{
	struct capi20_appl *ap;
	struct capi_ctr *ctr;
	u16 applid;

	mutex_lock(&capi_controller_lock);

	if (showcapimsgs & 1)
		printk(KERN_DEBUG "kcapi: notify up contr %d\n", contr);

	ctr = get_capi_ctr_by_nr(contr);
	if (ctr) {
		if (ctr->state == CAPI_CTR_RUNNING)
			goto unlock_out;

		ctr->state = CAPI_CTR_RUNNING;

		for (applid = 1; applid <= CAPI_MAXAPPL; applid++) {
			ap = __get_capi_appl_by_nr(applid);
			if (ap)
				register_appl(ctr, applid, &ap->rparam);
		}
	} else
		printk(KERN_WARNING "%s: invalid contr %d\n", __func__, contr);

unlock_out:
	mutex_unlock(&capi_controller_lock);
}