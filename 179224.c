static int rndis_keepalive_response(struct rndis_params *params,
				    rndis_keepalive_msg_type *buf)
{
	rndis_keepalive_cmplt_type *resp;
	rndis_resp_t *r;

	/* host "should" check only in RNDIS_DATA_INITIALIZED state */

	r = rndis_add_response(params, sizeof(rndis_keepalive_cmplt_type));
	if (!r)
		return -ENOMEM;
	resp = (rndis_keepalive_cmplt_type *)r->buf;

	resp->MessageType = cpu_to_le32(RNDIS_MSG_KEEPALIVE_C);
	resp->MessageLength = cpu_to_le32(16);
	resp->RequestID = buf->RequestID; /* Still LE in msg buffer */
	resp->Status = cpu_to_le32(RNDIS_STATUS_SUCCESS);

	params->resp_avail(params->v);
	return 0;
}