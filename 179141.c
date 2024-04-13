static int nfc_genl_enable_se(struct sk_buff *skb, struct genl_info *info)
{
	struct nfc_dev *dev;
	int rc;
	u32 idx, se_idx;

	if (!info->attrs[NFC_ATTR_DEVICE_INDEX] ||
	    !info->attrs[NFC_ATTR_SE_INDEX])
		return -EINVAL;

	idx = nla_get_u32(info->attrs[NFC_ATTR_DEVICE_INDEX]);
	se_idx = nla_get_u32(info->attrs[NFC_ATTR_SE_INDEX]);

	dev = nfc_get_device(idx);
	if (!dev)
		return -ENODEV;

	rc = nfc_enable_se(dev, se_idx);

	nfc_put_device(dev);
	return rc;
}