static int nfc_genl_stop_poll(struct sk_buff *skb, struct genl_info *info)
{
	struct nfc_dev *dev;
	int rc;
	u32 idx;

	if (!info->attrs[NFC_ATTR_DEVICE_INDEX])
		return -EINVAL;

	idx = nla_get_u32(info->attrs[NFC_ATTR_DEVICE_INDEX]);

	dev = nfc_get_device(idx);
	if (!dev)
		return -ENODEV;

	device_lock(&dev->dev);

	if (!dev->polling) {
		device_unlock(&dev->dev);
		nfc_put_device(dev);
		return -EINVAL;
	}

	device_unlock(&dev->dev);

	mutex_lock(&dev->genl_data.genl_data_mutex);

	if (dev->genl_data.poll_req_portid != info->snd_portid) {
		rc = -EBUSY;
		goto out;
	}

	rc = nfc_stop_poll(dev);
	dev->genl_data.poll_req_portid = 0;

out:
	mutex_unlock(&dev->genl_data.genl_data_mutex);
	nfc_put_device(dev);
	return rc;
}