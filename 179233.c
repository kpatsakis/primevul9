static int rndis_reset_response(struct rndis_params *params,
				rndis_reset_msg_type *buf)
{
	rndis_reset_cmplt_type *resp;
	rndis_resp_t *r;
	u8 *xbuf;
	u32 length;

	/* drain the response queue */
	while ((xbuf = rndis_get_next_response(params, &length)))
		rndis_free_response(params, xbuf);

	r = rndis_add_response(params, sizeof(rndis_reset_cmplt_type));
	if (!r)
		return -ENOMEM;
	resp = (rndis_reset_cmplt_type *)r->buf;

	resp->MessageType = cpu_to_le32(RNDIS_MSG_RESET_C);
	resp->MessageLength = cpu_to_le32(16);
	resp->Status = cpu_to_le32(RNDIS_STATUS_SUCCESS);
	/* resent information */
	resp->AddressingReset = cpu_to_le32(1);

	params->resp_avail(params->v);
	return 0;
}