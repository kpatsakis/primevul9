static ssize_t gadget_config_desc_MaxPower_show(struct config_item *item,
		char *page)
{
	return sprintf(page, "%u\n", to_config_usb_cfg(item)->c.MaxPower);
}