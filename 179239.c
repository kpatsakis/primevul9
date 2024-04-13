int rndis_set_param_dev(struct rndis_params *params, struct net_device *dev,
			u16 *cdc_filter)
{
	pr_debug("%s:\n", __func__);
	if (!dev)
		return -EINVAL;
	if (!params)
		return -1;

	params->dev = dev;
	params->filter = cdc_filter;

	return 0;
}