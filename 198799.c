*to_usb_os_desc_ext_prop(struct config_item *item)
{
	return container_of(item, struct usb_os_desc_ext_prop, item);
}