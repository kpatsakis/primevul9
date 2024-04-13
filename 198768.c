static ssize_t gadget_config_desc_MaxPower_store(struct config_item *item,
		const char *page, size_t len)
{
	u16 val;
	int ret;
	ret = kstrtou16(page, 0, &val);
	if (ret)
		return ret;
	if (DIV_ROUND_UP(val, 8) > 0xff)
		return -ERANGE;
	to_config_usb_cfg(item)->c.MaxPower = val;
	return len;
}