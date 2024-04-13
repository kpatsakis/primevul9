static void os_desc_unlink(struct config_item *os_desc_ci,
			  struct config_item *usb_cfg_ci)
{
	struct gadget_info *gi = container_of(to_config_group(os_desc_ci),
					struct gadget_info, os_desc_group);
	struct usb_composite_dev *cdev = &gi->cdev;

	mutex_lock(&gi->lock);
	if (gi->composite.gadget_driver.udc_name)
		unregister_gadget(gi);
	cdev->os_desc_config = NULL;
	WARN_ON(gi->composite.gadget_driver.udc_name);
	mutex_unlock(&gi->lock);
}