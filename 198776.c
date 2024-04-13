static ssize_t gadget_dev_desc_bcdDevice_store(struct config_item *item,
		const char *page, size_t len)
{
	u16 bcdDevice;
	int ret;

	ret = kstrtou16(page, 0, &bcdDevice);
	if (ret)
		return ret;
	ret = is_valid_bcd(bcdDevice);
	if (ret)
		return ret;

	to_gadget_info(item)->cdev.desc.bcdDevice = cpu_to_le16(bcdDevice);
	return len;
}