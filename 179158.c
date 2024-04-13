int nfc_genl_targets_found(struct nfc_dev *dev)
{
	struct sk_buff *msg;
	void *hdr;

	dev->genl_data.poll_req_portid = 0;

	msg = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_ATOMIC);
	if (!msg)
		return -ENOMEM;

	hdr = genlmsg_put(msg, 0, 0, &nfc_genl_family, 0,
			  NFC_EVENT_TARGETS_FOUND);
	if (!hdr)
		goto free_msg;

	if (nla_put_u32(msg, NFC_ATTR_DEVICE_INDEX, dev->idx))
		goto nla_put_failure;

	genlmsg_end(msg, hdr);

	return genlmsg_multicast(&nfc_genl_family, msg, 0, 0, GFP_ATOMIC);

nla_put_failure:
free_msg:
	nlmsg_free(msg);
	return -EMSGSIZE;
}