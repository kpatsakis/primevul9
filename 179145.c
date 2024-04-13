__nfc_alloc_vendor_cmd_skb(struct nfc_dev *dev, int approxlen,
			   u32 portid, u32 seq,
			   enum nfc_attrs attr,
			   u32 oui, u32 subcmd, gfp_t gfp)
{
	struct sk_buff *skb;
	void *hdr;

	skb = nlmsg_new(approxlen + 100, gfp);
	if (!skb)
		return NULL;

	hdr = nfc_hdr_put(skb, portid, seq, 0, NFC_CMD_VENDOR);
	if (!hdr) {
		kfree_skb(skb);
		return NULL;
	}

	if (nla_put_u32(skb, NFC_ATTR_DEVICE_INDEX, dev->idx))
		goto nla_put_failure;
	if (nla_put_u32(skb, NFC_ATTR_VENDOR_ID, oui))
		goto nla_put_failure;
	if (nla_put_u32(skb, NFC_ATTR_VENDOR_SUBCMD, subcmd))
		goto nla_put_failure;

	((void **)skb->cb)[0] = dev;
	((void **)skb->cb)[1] = hdr;

	return skb;

nla_put_failure:
	kfree_skb(skb);
	return NULL;
}