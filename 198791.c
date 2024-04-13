static ssize_t gadget_dev_desc_max_speed_show(struct config_item *item,
					      char *page)
{
	enum usb_device_speed speed = to_gadget_info(item)->composite.max_speed;

	return sprintf(page, "%s\n", usb_speed_string(speed));
}