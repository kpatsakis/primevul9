static ssize_t gadget_dev_desc_bcdUSB_store(struct config_item *item,
		const char *page, size_t len)
{
	u16 bcdUSB;
	int ret;

	ret = kstrtou16(page, 0, &bcdUSB);
	if (ret)
		return ret;
	ret = is_valid_bcd(bcdUSB);
	if (ret)
		return ret;

	to_gadget_info(item)->cdev.desc.bcdUSB = cpu_to_le16(bcdUSB);
	return len;
}