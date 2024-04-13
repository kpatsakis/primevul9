static int nfc_genl_se_io(struct sk_buff *skb, struct genl_info *info)
{
	struct nfc_dev *dev;
	struct se_io_ctx *ctx;
	u32 dev_idx, se_idx;
	u8 *apdu;
	size_t apdu_len;

	if (!info->attrs[NFC_ATTR_DEVICE_INDEX] ||
	    !info->attrs[NFC_ATTR_SE_INDEX] ||
	    !info->attrs[NFC_ATTR_SE_APDU])
		return -EINVAL;

	dev_idx = nla_get_u32(info->attrs[NFC_ATTR_DEVICE_INDEX]);
	se_idx = nla_get_u32(info->attrs[NFC_ATTR_SE_INDEX]);

	dev = nfc_get_device(dev_idx);
	if (!dev)
		return -ENODEV;

	if (!dev->ops || !dev->ops->se_io)
		return -ENOTSUPP;

	apdu_len = nla_len(info->attrs[NFC_ATTR_SE_APDU]);
	if (apdu_len == 0)
		return -EINVAL;

	apdu = nla_data(info->attrs[NFC_ATTR_SE_APDU]);
	if (!apdu)
		return -EINVAL;

	ctx = kzalloc(sizeof(struct se_io_ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->dev_idx = dev_idx;
	ctx->se_idx = se_idx;

	return nfc_se_io(dev, se_idx, apdu, apdu_len, se_io_cb, ctx);
}