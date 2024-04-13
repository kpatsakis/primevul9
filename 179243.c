void rndis_uninit(struct rndis_params *params)
{
	u8 *buf;
	u32 length;

	if (!params)
		return;
	params->state = RNDIS_UNINITIALIZED;

	/* drain the response queue */
	while ((buf = rndis_get_next_response(params, &length)))
		rndis_free_response(params, buf);
}