static int rndis_indicate_status_msg(struct rndis_params *params, u32 status)
{
	rndis_indicate_status_msg_type *resp;
	rndis_resp_t *r;

	if (params->state == RNDIS_UNINITIALIZED)
		return -ENOTSUPP;

	r = rndis_add_response(params, sizeof(rndis_indicate_status_msg_type));
	if (!r)
		return -ENOMEM;
	resp = (rndis_indicate_status_msg_type *)r->buf;

	resp->MessageType = cpu_to_le32(RNDIS_MSG_INDICATE);
	resp->MessageLength = cpu_to_le32(20);
	resp->Status = cpu_to_le32(status);
	resp->StatusBufferLength = cpu_to_le32(0);
	resp->StatusBufferOffset = cpu_to_le32(0);

	params->resp_avail(params->v);
	return 0;
}