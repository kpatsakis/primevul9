static ssize_t ext_prop_data_show(struct config_item *item, char *page)
{
	struct usb_os_desc_ext_prop *ext_prop = to_usb_os_desc_ext_prop(item);
	int len = ext_prop->data_len;

	if (ext_prop->type == USB_EXT_PROP_UNICODE ||
	    ext_prop->type == USB_EXT_PROP_UNICODE_ENV ||
	    ext_prop->type == USB_EXT_PROP_UNICODE_LINK)
		len >>= 1;
	memcpy(page, ext_prop->data, len);

	return len;
}