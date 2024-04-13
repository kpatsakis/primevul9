int rndis_set_param_medium(struct rndis_params *params, u32 medium, u32 speed)
{
	pr_debug("%s: %u %u\n", __func__, medium, speed);
	if (!params)
		return -1;

	params->medium = medium;
	params->speed = speed;

	return 0;
}