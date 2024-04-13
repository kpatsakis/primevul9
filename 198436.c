int sc_file_set_type_attr(sc_file_t *file, const u8 *type_attr,
			 size_t type_attr_len)
{
	u8 *tmp;
	if (!sc_file_valid(file)) {
		return SC_ERROR_INVALID_ARGUMENTS;
	}

	if (type_attr == NULL) {
		if (file->type_attr != NULL)
			free(file->type_attr);
		file->type_attr = NULL;
		file->type_attr_len = 0;
		return SC_SUCCESS;
	 }
	tmp = (u8 *) realloc(file->type_attr, type_attr_len);
	if (!tmp) {
		if (file->type_attr)
			free(file->type_attr);
		file->type_attr = NULL;
		file->type_attr_len = 0;
		return SC_ERROR_OUT_OF_MEMORY;
	}
	file->type_attr = tmp;
	memcpy(file->type_attr, type_attr, type_attr_len);
	file->type_attr_len = type_attr_len;

	return SC_SUCCESS;
}