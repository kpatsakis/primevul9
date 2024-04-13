static int _pam_get_item(const pam_handle_t *pamh,
			 int item_type,
			 const void *_item)
{
	const void **item = (const void **)_item;
	return pam_get_item(pamh, item_type, item);
}