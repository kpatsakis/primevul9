static ssize_t interf_grp_compatible_id_show(struct config_item *item,
					     char *page)
{
	memcpy(page, to_usb_os_desc(item)->ext_compat_id, 8);
	return 8;
}