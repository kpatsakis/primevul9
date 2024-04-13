static ssize_t gadget_dev_desc_max_speed_store(struct config_item *item,
					       const char *page, size_t len)
{
	struct gadget_info *gi = to_gadget_info(item);

	mutex_lock(&gi->lock);

	/* Prevent changing of max_speed after the driver is binded */
	if (gi->composite.gadget_driver.udc_name)
		goto err;

	if (strncmp(page, "super-speed-plus", 16) == 0)
		gi->composite.max_speed = USB_SPEED_SUPER_PLUS;
	else if (strncmp(page, "super-speed", 11) == 0)
		gi->composite.max_speed = USB_SPEED_SUPER;
	else if (strncmp(page, "high-speed", 10) == 0)
		gi->composite.max_speed = USB_SPEED_HIGH;
	else if (strncmp(page, "full-speed", 10) == 0)
		gi->composite.max_speed = USB_SPEED_FULL;
	else if (strncmp(page, "low-speed", 9) == 0)
		gi->composite.max_speed = USB_SPEED_LOW;
	else
		goto err;

	gi->composite.gadget_driver.max_speed = gi->composite.max_speed;

	mutex_unlock(&gi->lock);
	return len;
err:
	mutex_unlock(&gi->lock);
	return -EINVAL;
}