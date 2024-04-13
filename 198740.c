static void config_usb_cfg_unlink(
	struct config_item *usb_cfg_ci,
	struct config_item *usb_func_ci)
{
	struct config_usb_cfg *cfg = to_config_usb_cfg(usb_cfg_ci);
	struct usb_composite_dev *cdev = cfg->c.cdev;
	struct gadget_info *gi = container_of(cdev, struct gadget_info, cdev);

	struct config_group *group = to_config_group(usb_func_ci);
	struct usb_function_instance *fi = container_of(group,
			struct usb_function_instance, group);
	struct usb_function *f;

	/*
	 * ideally I would like to forbid to unlink functions while a gadget is
	 * bound to an UDC. Since this isn't possible at the moment, we simply
	 * force an unbind, the function is available here and then we can
	 * remove the function.
	 */
	mutex_lock(&gi->lock);
	if (gi->composite.gadget_driver.udc_name)
		unregister_gadget(gi);
	WARN_ON(gi->composite.gadget_driver.udc_name);

	list_for_each_entry(f, &cfg->func_list, list) {
		if (f->fi == fi) {
			list_del(&f->list);
			usb_put_function(f);
			mutex_unlock(&gi->lock);
			return;
		}
	}
	mutex_unlock(&gi->lock);
	WARN(1, "Unable to locate function to unbind\n");
}