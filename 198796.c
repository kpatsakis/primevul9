static ssize_t ext_prop_type_show(struct config_item *item, char *page)
{
	return sprintf(page, "%d\n", to_usb_os_desc_ext_prop(item)->type);
}