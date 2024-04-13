static struct config_item *ext_prop_make(
		struct config_group *group,
		const char *name)
{
	struct usb_os_desc_ext_prop *ext_prop;
	struct config_item_type *ext_prop_type;
	struct usb_os_desc *desc;
	char *vlabuf;

	vla_group(data_chunk);
	vla_item(data_chunk, struct usb_os_desc_ext_prop, ext_prop, 1);
	vla_item(data_chunk, struct config_item_type, ext_prop_type, 1);

	vlabuf = kzalloc(vla_group_size(data_chunk), GFP_KERNEL);
	if (!vlabuf)
		return ERR_PTR(-ENOMEM);

	ext_prop = vla_ptr(vlabuf, data_chunk, ext_prop);
	ext_prop_type = vla_ptr(vlabuf, data_chunk, ext_prop_type);

	desc = container_of(group, struct usb_os_desc, group);
	ext_prop_type->ct_item_ops = &ext_prop_ops;
	ext_prop_type->ct_attrs = ext_prop_attrs;
	ext_prop_type->ct_owner = desc->owner;

	config_item_init_type_name(&ext_prop->item, name, ext_prop_type);

	ext_prop->name = kstrdup(name, GFP_KERNEL);
	if (!ext_prop->name) {
		kfree(vlabuf);
		return ERR_PTR(-ENOMEM);
	}
	desc->ext_prop_len += 14;
	ext_prop->name_len = 2 * strlen(ext_prop->name) + 2;
	if (desc->opts_mutex)
		mutex_lock(desc->opts_mutex);
	desc->ext_prop_len += ext_prop->name_len;
	list_add_tail(&ext_prop->entry, &desc->ext_prop);
	++desc->ext_prop_count;
	if (desc->opts_mutex)
		mutex_unlock(desc->opts_mutex);

	return &ext_prop->item;
}