static void ext_prop_drop(struct config_group *group, struct config_item *item)
{
	struct usb_os_desc_ext_prop *ext_prop = to_usb_os_desc_ext_prop(item);
	struct usb_os_desc *desc = to_usb_os_desc(&group->cg_item);

	if (desc->opts_mutex)
		mutex_lock(desc->opts_mutex);
	list_del(&ext_prop->entry);
	--desc->ext_prop_count;
	kfree(ext_prop->name);
	desc->ext_prop_len -= (ext_prop->name_len + ext_prop->data_len + 14);
	if (desc->opts_mutex)
		mutex_unlock(desc->opts_mutex);
	config_item_put(item);
}