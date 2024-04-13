void rndis_deregister(struct rndis_params *params)
{
	int i;

	pr_debug("%s:\n", __func__);

	if (!params)
		return;

	i = params->confignr;

#ifdef CONFIG_USB_GADGET_DEBUG_FILES
	{
		char name[20];

		sprintf(name, NAME_TEMPLATE, i);
		remove_proc_entry(name, NULL);
	}
#endif

	kfree(params);
	rndis_put_nr(i);
}