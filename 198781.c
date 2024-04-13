struct config_group *usb_os_desc_prepare_interf_dir(
		struct config_group *parent,
		int n_interf,
		struct usb_os_desc **desc,
		char **names,
		struct module *owner)
{
	struct config_group *os_desc_group;
	struct config_item_type *os_desc_type, *interface_type;

	vla_group(data_chunk);
	vla_item(data_chunk, struct config_group, os_desc_group, 1);
	vla_item(data_chunk, struct config_item_type, os_desc_type, 1);
	vla_item(data_chunk, struct config_item_type, interface_type, 1);

	char *vlabuf = kzalloc(vla_group_size(data_chunk), GFP_KERNEL);
	if (!vlabuf)
		return ERR_PTR(-ENOMEM);

	os_desc_group = vla_ptr(vlabuf, data_chunk, os_desc_group);
	os_desc_type = vla_ptr(vlabuf, data_chunk, os_desc_type);
	interface_type = vla_ptr(vlabuf, data_chunk, interface_type);

	os_desc_type->ct_owner = owner;
	config_group_init_type_name(os_desc_group, "os_desc", os_desc_type);
	configfs_add_default_group(os_desc_group, parent);

	interface_type->ct_group_ops = &interf_grp_ops;
	interface_type->ct_attrs = interf_grp_attrs;
	interface_type->ct_owner = owner;

	while (n_interf--) {
		struct usb_os_desc *d;

		d = desc[n_interf];
		d->owner = owner;
		config_group_init_type_name(&d->group, "", interface_type);
		config_item_set_name(&d->group.cg_item, "interface.%s",
				     names[n_interf]);
		configfs_add_default_group(&d->group, os_desc_group);
	}

	return os_desc_group;
}