int rndis_set_param_vendor(struct rndis_params *params, u32 vendorID,
			   const char *vendorDescr)
{
	pr_debug("%s:\n", __func__);
	if (!vendorDescr) return -1;
	if (!params)
		return -1;

	params->vendorID = vendorID;
	params->vendorDescr = vendorDescr;

	return 0;
}