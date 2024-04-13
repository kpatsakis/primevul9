static int nfc_genl_setup_device_added(struct nfc_dev *dev, struct sk_buff *msg)
{
	if (nla_put_string(msg, NFC_ATTR_DEVICE_NAME, nfc_device_name(dev)) ||
	    nla_put_u32(msg, NFC_ATTR_DEVICE_INDEX, dev->idx) ||
	    nla_put_u32(msg, NFC_ATTR_PROTOCOLS, dev->supported_protocols) ||
	    nla_put_u8(msg, NFC_ATTR_DEVICE_POWERED, dev->dev_up) ||
	    nla_put_u8(msg, NFC_ATTR_RF_MODE, dev->rf_mode))
		return -1;
	return 0;
}