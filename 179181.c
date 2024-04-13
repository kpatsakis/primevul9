static int nfc_genl_vendor_cmd(struct sk_buff *skb,
			       struct genl_info *info)
{
	struct nfc_dev *dev;
	const struct nfc_vendor_cmd *cmd;
	u32 dev_idx, vid, subcmd;
	u8 *data;
	size_t data_len;
	int i, err;

	if (!info->attrs[NFC_ATTR_DEVICE_INDEX] ||
	    !info->attrs[NFC_ATTR_VENDOR_ID] ||
	    !info->attrs[NFC_ATTR_VENDOR_SUBCMD])
		return -EINVAL;

	dev_idx = nla_get_u32(info->attrs[NFC_ATTR_DEVICE_INDEX]);
	vid = nla_get_u32(info->attrs[NFC_ATTR_VENDOR_ID]);
	subcmd = nla_get_u32(info->attrs[NFC_ATTR_VENDOR_SUBCMD]);

	dev = nfc_get_device(dev_idx);
	if (!dev || !dev->vendor_cmds || !dev->n_vendor_cmds)
		return -ENODEV;

	if (info->attrs[NFC_ATTR_VENDOR_DATA]) {
		data = nla_data(info->attrs[NFC_ATTR_VENDOR_DATA]);
		data_len = nla_len(info->attrs[NFC_ATTR_VENDOR_DATA]);
		if (data_len == 0)
			return -EINVAL;
	} else {
		data = NULL;
		data_len = 0;
	}

	for (i = 0; i < dev->n_vendor_cmds; i++) {
		cmd = &dev->vendor_cmds[i];

		if (cmd->vendor_id != vid || cmd->subcmd != subcmd)
			continue;

		dev->cur_cmd_info = info;
		err = cmd->doit(dev, data, data_len);
		dev->cur_cmd_info = NULL;
		return err;
	}

	return -EOPNOTSUPP;
}