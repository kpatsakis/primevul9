static int nfc_genl_activate_target(struct sk_buff *skb, struct genl_info *info)
{
	struct nfc_dev *dev;
	u32 device_idx, target_idx, protocol;
	int rc;

	if (!info->attrs[NFC_ATTR_DEVICE_INDEX] ||
	    !info->attrs[NFC_ATTR_TARGET_INDEX] ||
	    !info->attrs[NFC_ATTR_PROTOCOLS])
		return -EINVAL;

	device_idx = nla_get_u32(info->attrs[NFC_ATTR_DEVICE_INDEX]);

	dev = nfc_get_device(device_idx);
	if (!dev)
		return -ENODEV;

	target_idx = nla_get_u32(info->attrs[NFC_ATTR_TARGET_INDEX]);
	protocol = nla_get_u32(info->attrs[NFC_ATTR_PROTOCOLS]);

	nfc_deactivate_target(dev, target_idx, NFC_TARGET_MODE_SLEEP);
	rc = nfc_activate_target(dev, target_idx, protocol);

	nfc_put_device(dev);
	return rc;
}