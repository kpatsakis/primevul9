struct rndis_params *rndis_register(void (*resp_avail)(void *v), void *v)
{
	struct rndis_params *params;
	int i;

	if (!resp_avail)
		return ERR_PTR(-EINVAL);

	i = rndis_get_nr();
	if (i < 0) {
		pr_debug("failed\n");

		return ERR_PTR(-ENODEV);
	}

	params = kzalloc(sizeof(*params), GFP_KERNEL);
	if (!params) {
		rndis_put_nr(i);

		return ERR_PTR(-ENOMEM);
	}

#ifdef	CONFIG_USB_GADGET_DEBUG_FILES
	{
		struct proc_dir_entry *proc_entry;
		char name[20];

		sprintf(name, NAME_TEMPLATE, i);
		proc_entry = proc_create_data(name, 0660, NULL,
					      &rndis_proc_ops, params);
		if (!proc_entry) {
			kfree(params);
			rndis_put_nr(i);

			return ERR_PTR(-EIO);
		}
	}
#endif

	params->confignr = i;
	params->used = 1;
	params->state = RNDIS_UNINITIALIZED;
	params->media_state = RNDIS_MEDIA_STATE_DISCONNECTED;
	params->resp_avail = resp_avail;
	params->v = v;
	INIT_LIST_HEAD(&params->resp_queue);
	pr_debug("%s: configNr = %d\n", __func__, i);

	return params;
}