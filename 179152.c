static int nfc_genl_send_se(struct sk_buff *msg, struct nfc_dev *dev,
				u32 portid, u32 seq,
				struct netlink_callback *cb,
				int flags)
{
	void *hdr;
	struct nfc_se *se, *n;

	list_for_each_entry_safe(se, n, &dev->secure_elements, list) {
		hdr = genlmsg_put(msg, portid, seq, &nfc_genl_family, flags,
				  NFC_CMD_GET_SE);
		if (!hdr)
			goto nla_put_failure;

		if (cb)
			genl_dump_check_consistent(cb, hdr);

		if (nla_put_u32(msg, NFC_ATTR_DEVICE_INDEX, dev->idx) ||
		    nla_put_u32(msg, NFC_ATTR_SE_INDEX, se->idx) ||
		    nla_put_u8(msg, NFC_ATTR_SE_TYPE, se->type))
			goto nla_put_failure;

		genlmsg_end(msg, hdr);
	}

	return 0;

nla_put_failure:
	genlmsg_cancel(msg, hdr);
	return -EMSGSIZE;
}