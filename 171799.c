static int _pam_get_data(const pam_handle_t *pamh,
			 const char *module_data_name,
			 const void *_data)
{
	const void **data = (const void **)_data;
	return pam_get_data(pamh, module_data_name, data);
}