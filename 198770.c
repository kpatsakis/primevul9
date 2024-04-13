static void function_drop(
		struct config_group *group,
		struct config_item *item)
{
	struct usb_function_instance *fi = to_usb_function_instance(item);
	struct gadget_info *gi;

	gi = container_of(group, struct gadget_info, functions_group);

	mutex_lock(&gi->lock);
	list_del(&fi->cfs_list);
	mutex_unlock(&gi->lock);
	config_item_put(item);
}