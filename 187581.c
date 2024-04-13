static int sas_discover_expander(struct domain_device *dev)
{
	int res;

	res = sas_notify_lldd_dev_found(dev);
	if (res)
		return res;

	res = sas_ex_general(dev);
	if (res)
		goto out_err;
	res = sas_ex_manuf_info(dev);
	if (res)
		goto out_err;

	res = sas_expander_discover(dev);
	if (res) {
		pr_warn("expander %016llx discovery failed(0x%x)\n",
			SAS_ADDR(dev->sas_addr), res);
		goto out_err;
	}

	sas_check_ex_subtractive_boundary(dev);
	res = sas_check_parent_topology(dev);
	if (res)
		goto out_err;
	return 0;
out_err:
	sas_notify_lldd_dev_gone(dev);
	return res;
}