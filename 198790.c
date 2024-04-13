static int config_usb_cfg_link(
	struct config_item *usb_cfg_ci,
	struct config_item *usb_func_ci)
{
	struct config_usb_cfg *cfg = to_config_usb_cfg(usb_cfg_ci);
	struct usb_composite_dev *cdev = cfg->c.cdev;
	struct gadget_info *gi = container_of(cdev, struct gadget_info, cdev);

	struct config_group *group = to_config_group(usb_func_ci);
	struct usb_function_instance *fi = container_of(group,
			struct usb_function_instance, group);
	struct usb_function_instance *a_fi;
	struct usb_function *f;
	int ret;

	mutex_lock(&gi->lock);
	/*
	 * Make sure this function is from within our _this_ gadget and not
	 * from another gadget or a random directory.
	 * Also a function instance can only be linked once.
	 */
	list_for_each_entry(a_fi, &gi->available_func, cfs_list) {
		if (a_fi == fi)
			break;
	}
	if (a_fi != fi) {
		ret = -EINVAL;
		goto out;
	}

	list_for_each_entry(f, &cfg->func_list, list) {
		if (f->fi == fi) {
			ret = -EEXIST;
			goto out;
		}
	}

	f = usb_get_function(fi);
	if (IS_ERR(f)) {
		ret = PTR_ERR(f);
		goto out;
	}

	/* stash the function until we bind it to the gadget */
	list_add_tail(&f->list, &cfg->func_list);
	ret = 0;
out:
	mutex_unlock(&gi->lock);
	return ret;
}