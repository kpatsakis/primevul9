static int os_desc_link(struct config_item *os_desc_ci,
			struct config_item *usb_cfg_ci)
{
	struct gadget_info *gi = container_of(to_config_group(os_desc_ci),
					struct gadget_info, os_desc_group);
	struct usb_composite_dev *cdev = &gi->cdev;
	struct config_usb_cfg *c_target =
		container_of(to_config_group(usb_cfg_ci),
			     struct config_usb_cfg, group);
	struct usb_configuration *c;
	int ret;

	mutex_lock(&gi->lock);
	list_for_each_entry(c, &cdev->configs, list) {
		if (c == &c_target->c)
			break;
	}
	if (c != &c_target->c) {
		ret = -EINVAL;
		goto out;
	}

	if (cdev->os_desc_config) {
		ret = -EBUSY;
		goto out;
	}

	cdev->os_desc_config = &c_target->c;
	ret = 0;

out:
	mutex_unlock(&gi->lock);
	return ret;
}