static ssize_t gadget_config_desc_bmAttributes_show(struct config_item *item,
		char *page)
{
	return sprintf(page, "0x%02x\n",
		to_config_usb_cfg(item)->c.bmAttributes);
}