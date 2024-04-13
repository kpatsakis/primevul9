int nfc_vendor_cmd_reply(struct sk_buff *skb)
{
	struct nfc_dev *dev = ((void **)skb->cb)[0];
	void *hdr = ((void **)skb->cb)[1];

	/* clear CB data for netlink core to own from now on */
	memset(skb->cb, 0, sizeof(skb->cb));

	if (WARN_ON(!dev->cur_cmd_info)) {
		kfree_skb(skb);
		return -EINVAL;
	}

	genlmsg_end(skb, hdr);
	return genlmsg_reply(skb, dev->cur_cmd_info);
}