static ssize_t ext_prop_type_store(struct config_item *item,
				   const char *page, size_t len)
{
	struct usb_os_desc_ext_prop *ext_prop = to_usb_os_desc_ext_prop(item);
	struct usb_os_desc *desc = to_usb_os_desc(ext_prop->item.ci_parent);
	u8 type;
	int ret;

	if (desc->opts_mutex)
		mutex_lock(desc->opts_mutex);
	ret = kstrtou8(page, 0, &type);
	if (ret)
		goto end;
	if (type < USB_EXT_PROP_UNICODE || type > USB_EXT_PROP_UNICODE_MULTI) {
		ret = -EINVAL;
		goto end;
	}

	if ((ext_prop->type == USB_EXT_PROP_BINARY ||
	    ext_prop->type == USB_EXT_PROP_LE32 ||
	    ext_prop->type == USB_EXT_PROP_BE32) &&
	    (type == USB_EXT_PROP_UNICODE ||
	    type == USB_EXT_PROP_UNICODE_ENV ||
	    type == USB_EXT_PROP_UNICODE_LINK))
		ext_prop->data_len <<= 1;
	else if ((ext_prop->type == USB_EXT_PROP_UNICODE ||
		   ext_prop->type == USB_EXT_PROP_UNICODE_ENV ||
		   ext_prop->type == USB_EXT_PROP_UNICODE_LINK) &&
		   (type == USB_EXT_PROP_BINARY ||
		   type == USB_EXT_PROP_LE32 ||
		   type == USB_EXT_PROP_BE32))
		ext_prop->data_len >>= 1;
	ext_prop->type = type;
	ret = len;

end:
	if (desc->opts_mutex)
		mutex_unlock(desc->opts_mutex);
	return ret;
}