int nfc_genl_device_added(struct nfc_dev *dev)
{
	struct sk_buff *msg;
	void *hdr;

	msg = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!msg)
		return -ENOMEM;

	hdr = genlmsg_put(msg, 0, 0, &nfc_genl_family, 0,
			  NFC_EVENT_DEVICE_ADDED);
	if (!hdr)
		goto free_msg;

	if (nfc_genl_setup_device_added(dev, msg))
		goto nla_put_failure;

	genlmsg_end(msg, hdr);

	genlmsg_multicast(&nfc_genl_family, msg, 0, 0, GFP_KERNEL);

	return 0;

nla_put_failure:
free_msg:
	nlmsg_free(msg);
	return -EMSGSIZE;
}