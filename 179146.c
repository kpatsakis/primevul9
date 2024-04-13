static struct nfc_dev *__get_device_from_cb(struct netlink_callback *cb)
{
	const struct genl_dumpit_info *info = genl_dumpit_info(cb);
	struct nfc_dev *dev;
	u32 idx;

	if (!info->attrs[NFC_ATTR_DEVICE_INDEX])
		return ERR_PTR(-EINVAL);

	idx = nla_get_u32(info->attrs[NFC_ATTR_DEVICE_INDEX]);

	dev = nfc_get_device(idx);
	if (!dev)
		return ERR_PTR(-ENODEV);

	return dev;
}