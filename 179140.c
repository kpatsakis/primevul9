static int nfc_genl_send_params(struct sk_buff *msg,
				struct nfc_llcp_local *local,
				u32 portid, u32 seq)
{
	void *hdr;

	hdr = genlmsg_put(msg, portid, seq, &nfc_genl_family, 0,
			  NFC_CMD_LLC_GET_PARAMS);
	if (!hdr)
		return -EMSGSIZE;

	if (nla_put_u32(msg, NFC_ATTR_DEVICE_INDEX, local->dev->idx) ||
	    nla_put_u8(msg, NFC_ATTR_LLC_PARAM_LTO, local->lto) ||
	    nla_put_u8(msg, NFC_ATTR_LLC_PARAM_RW, local->rw) ||
	    nla_put_u16(msg, NFC_ATTR_LLC_PARAM_MIUX, be16_to_cpu(local->miux)))
		goto nla_put_failure;

	genlmsg_end(msg, hdr);
	return 0;

nla_put_failure:
	genlmsg_cancel(msg, hdr);
	return -EMSGSIZE;
}