static ssize_t gadget_config_desc_bmAttributes_store(struct config_item *item,
		const char *page, size_t len)
{
	u8 val;
	int ret;
	ret = kstrtou8(page, 0, &val);
	if (ret)
		return ret;
	if (!(val & USB_CONFIG_ATT_ONE))
		return -EINVAL;
	if (val & ~(USB_CONFIG_ATT_ONE | USB_CONFIG_ATT_SELFPOWER |
				USB_CONFIG_ATT_WAKEUP))
		return -EINVAL;
	to_config_usb_cfg(item)->c.bmAttributes = val;
	return len;
}