static void notify_down(u32 contr)
{
	struct capi_ctr *ctr;

	mutex_lock(&capi_controller_lock);

	if (showcapimsgs & 1)
		printk(KERN_DEBUG "kcapi: notify down contr %d\n", contr);

	ctr = get_capi_ctr_by_nr(contr);
	if (ctr)
		ctr_down(ctr, CAPI_CTR_DETECTED);
	else
		printk(KERN_WARNING "%s: invalid contr %d\n", __func__, contr);

	mutex_unlock(&capi_controller_lock);
}