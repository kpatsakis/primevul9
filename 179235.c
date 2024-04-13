static int rndis_query_response(struct rndis_params *params,
				rndis_query_msg_type *buf)
{
	rndis_query_cmplt_type *resp;
	rndis_resp_t *r;

	/* pr_debug("%s: OID = %08X\n", __func__, cpu_to_le32(buf->OID)); */
	if (!params->dev)
		return -ENOTSUPP;

	/*
	 * we need more memory:
	 * gen_ndis_query_resp expects enough space for
	 * rndis_query_cmplt_type followed by data.
	 * oid_supported_list is the largest data reply
	 */
	r = rndis_add_response(params,
		sizeof(oid_supported_list) + sizeof(rndis_query_cmplt_type));
	if (!r)
		return -ENOMEM;
	resp = (rndis_query_cmplt_type *)r->buf;

	resp->MessageType = cpu_to_le32(RNDIS_MSG_QUERY_C);
	resp->RequestID = buf->RequestID; /* Still LE in msg buffer */

	if (gen_ndis_query_resp(params, le32_to_cpu(buf->OID),
			le32_to_cpu(buf->InformationBufferOffset)
					+ 8 + (u8 *)buf,
			le32_to_cpu(buf->InformationBufferLength),
			r)) {
		/* OID not supported */
		resp->Status = cpu_to_le32(RNDIS_STATUS_NOT_SUPPORTED);
		resp->MessageLength = cpu_to_le32(sizeof *resp);
		resp->InformationBufferLength = cpu_to_le32(0);
		resp->InformationBufferOffset = cpu_to_le32(0);
	} else
		resp->Status = cpu_to_le32(RNDIS_STATUS_SUCCESS);

	params->resp_avail(params->v);
	return 0;
}