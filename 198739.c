static ssize_t interf_grp_compatible_id_store(struct config_item *item,
					      const char *page, size_t len)
{
	struct usb_os_desc *desc = to_usb_os_desc(item);
	int l;

	l = min_t(int, 8, len);
	if (page[l - 1] == '\n')
		--l;
	if (desc->opts_mutex)
		mutex_lock(desc->opts_mutex);
	memcpy(desc->ext_compat_id, page, l);

	if (desc->opts_mutex)
		mutex_unlock(desc->opts_mutex);

	return len;
}