struct sk_buff *__nfc_alloc_vendor_cmd_reply_skb(struct nfc_dev *dev,
						 enum nfc_attrs attr,
						 u32 oui, u32 subcmd,
						 int approxlen)
{
	if (WARN_ON(!dev->cur_cmd_info))
		return NULL;

	return __nfc_alloc_vendor_cmd_skb(dev, approxlen,
					  dev->cur_cmd_info->snd_portid,
					  dev->cur_cmd_info->snd_seq, attr,
					  oui, subcmd, GFP_KERNEL);
}