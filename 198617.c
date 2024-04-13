int sc_file_set_prop_attr(sc_file_t *file, const u8 *prop_attr,
			 size_t prop_attr_len)
{
	u8 *tmp;
	if (!sc_file_valid(file)) {
		return SC_ERROR_INVALID_ARGUMENTS;
	}

	if (prop_attr == NULL) {
		if (file->prop_attr != NULL)
			free(file->prop_attr);
		file->prop_attr = NULL;
		file->prop_attr_len = 0;
		return SC_SUCCESS;
	 }
	tmp = (u8 *) realloc(file->prop_attr, prop_attr_len);
	if (!tmp) {
		if (file->prop_attr)
			free(file->prop_attr);
		file->prop_attr = NULL;
		file->prop_attr_len = 0;
		return SC_ERROR_OUT_OF_MEMORY;
	}
	file->prop_attr = tmp;
	memcpy(file->prop_attr, prop_attr, prop_attr_len);
	file->prop_attr_len = prop_attr_len;

	return SC_SUCCESS;
}