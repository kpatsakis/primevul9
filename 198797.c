static struct config_group *function_make(
		struct config_group *group,
		const char *name)
{
	struct gadget_info *gi;
	struct usb_function_instance *fi;
	char buf[MAX_NAME_LEN];
	char *func_name;
	char *instance_name;
	int ret;

	ret = snprintf(buf, MAX_NAME_LEN, "%s", name);
	if (ret >= MAX_NAME_LEN)
		return ERR_PTR(-ENAMETOOLONG);

	func_name = buf;
	instance_name = strchr(func_name, '.');
	if (!instance_name) {
		pr_err("Unable to locate . in FUNC.INSTANCE\n");
		return ERR_PTR(-EINVAL);
	}
	*instance_name = '\0';
	instance_name++;

	fi = usb_get_function_instance(func_name);
	if (IS_ERR(fi))
		return ERR_CAST(fi);

	ret = config_item_set_name(&fi->group.cg_item, "%s", name);
	if (ret) {
		usb_put_function_instance(fi);
		return ERR_PTR(ret);
	}
	if (fi->set_inst_name) {
		ret = fi->set_inst_name(fi, instance_name);
		if (ret) {
			usb_put_function_instance(fi);
			return ERR_PTR(ret);
		}
	}

	gi = container_of(group, struct gadget_info, functions_group);

	mutex_lock(&gi->lock);
	list_add_tail(&fi->cfs_list, &gi->available_func);
	mutex_unlock(&gi->lock);
	return &fi->group;
}