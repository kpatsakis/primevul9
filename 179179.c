int nfc_genl_target_lost(struct nfc_dev *dev, u32 target_idx)
{
	struct sk_buff *msg;
	void *hdr;

	msg = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!msg)
		return -ENOMEM;

	hdr = genlmsg_put(msg, 0, 0, &nfc_genl_family, 0,
			  NFC_EVENT_TARGET_LOST);
	if (!hdr)
		goto free_msg;

	if (nla_put_string(msg, NFC_ATTR_DEVICE_NAME, nfc_device_name(dev)) ||
	    nla_put_u32(msg, NFC_ATTR_TARGET_INDEX, target_idx))
		goto nla_put_failure;

	genlmsg_end(msg, hdr);

	genlmsg_multicast(&nfc_genl_family, msg, 0, 0, GFP_KERNEL);

	return 0;

nla_put_failure:
free_msg:
	nlmsg_free(msg);
	return -EMSGSIZE;
}