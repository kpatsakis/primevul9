static int nfc_genl_fw_download(struct sk_buff *skb, struct genl_info *info)
{
	struct nfc_dev *dev;
	int rc;
	u32 idx;
	char firmware_name[NFC_FIRMWARE_NAME_MAXSIZE + 1];

	if (!info->attrs[NFC_ATTR_DEVICE_INDEX] || !info->attrs[NFC_ATTR_FIRMWARE_NAME])
		return -EINVAL;

	idx = nla_get_u32(info->attrs[NFC_ATTR_DEVICE_INDEX]);

	dev = nfc_get_device(idx);
	if (!dev)
		return -ENODEV;

	nla_strscpy(firmware_name, info->attrs[NFC_ATTR_FIRMWARE_NAME],
		    sizeof(firmware_name));

	rc = nfc_fw_download(dev, firmware_name);

	nfc_put_device(dev);
	return rc;
}