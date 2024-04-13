static ssize_t ext_prop_data_store(struct config_item *item,
				   const char *page, size_t len)
{
	struct usb_os_desc_ext_prop *ext_prop = to_usb_os_desc_ext_prop(item);
	struct usb_os_desc *desc = to_usb_os_desc(ext_prop->item.ci_parent);
	char *new_data;
	size_t ret_len = len;

	if (page[len - 1] == '\n' || page[len - 1] == '\0')
		--len;
	new_data = kmemdup(page, len, GFP_KERNEL);
	if (!new_data)
		return -ENOMEM;

	if (desc->opts_mutex)
		mutex_lock(desc->opts_mutex);
	kfree(ext_prop->data);
	ext_prop->data = new_data;
	desc->ext_prop_len -= ext_prop->data_len;
	ext_prop->data_len = len;
	desc->ext_prop_len += ext_prop->data_len;
	if (ext_prop->type == USB_EXT_PROP_UNICODE ||
	    ext_prop->type == USB_EXT_PROP_UNICODE_ENV ||
	    ext_prop->type == USB_EXT_PROP_UNICODE_LINK) {
		desc->ext_prop_len -= ext_prop->data_len;
		ext_prop->data_len <<= 1;
		ext_prop->data_len += 2;
		desc->ext_prop_len += ext_prop->data_len;
	}
	if (desc->opts_mutex)
		mutex_unlock(desc->opts_mutex);
	return ret_len;
}