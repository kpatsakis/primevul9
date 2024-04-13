register_appl(struct capi_ctr *ctr, u16 applid, capi_register_params *rparam)
{
	ctr = capi_ctr_get(ctr);

	if (ctr)
		ctr->register_appl(ctr, applid, rparam);
	else
		printk(KERN_WARNING "%s: cannot get controller resources\n",
		       __func__);
}