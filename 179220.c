static int rndis_init_response(struct rndis_params *params,
			       rndis_init_msg_type *buf)
{
	rndis_init_cmplt_type *resp;
	rndis_resp_t *r;

	if (!params->dev)
		return -ENOTSUPP;

	r = rndis_add_response(params, sizeof(rndis_init_cmplt_type));
	if (!r)
		return -ENOMEM;
	resp = (rndis_init_cmplt_type *)r->buf;

	resp->MessageType = cpu_to_le32(RNDIS_MSG_INIT_C);
	resp->MessageLength = cpu_to_le32(52);
	resp->RequestID = buf->RequestID; /* Still LE in msg buffer */
	resp->Status = cpu_to_le32(RNDIS_STATUS_SUCCESS);
	resp->MajorVersion = cpu_to_le32(RNDIS_MAJOR_VERSION);
	resp->MinorVersion = cpu_to_le32(RNDIS_MINOR_VERSION);
	resp->DeviceFlags = cpu_to_le32(RNDIS_DF_CONNECTIONLESS);
	resp->Medium = cpu_to_le32(RNDIS_MEDIUM_802_3);
	resp->MaxPacketsPerTransfer = cpu_to_le32(1);
	resp->MaxTransferSize = cpu_to_le32(
		  params->dev->mtu
		+ sizeof(struct ethhdr)
		+ sizeof(struct rndis_packet_msg_type)
		+ 22);
	resp->PacketAlignmentFactor = cpu_to_le32(0);
	resp->AFListOffset = cpu_to_le32(0);
	resp->AFListSize = cpu_to_le32(0);

	params->resp_avail(params->v);
	return 0;
}