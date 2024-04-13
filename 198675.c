int sc_file_set_content(sc_file_t *file, const u8 *content,
			 size_t content_len)
{
	u8 *tmp;
	if (!sc_file_valid(file)) {
		return SC_ERROR_INVALID_ARGUMENTS;
	}

	if (content == NULL) {
		if (file->encoded_content != NULL)
			free(file->encoded_content);
		file->encoded_content = NULL;
		file->encoded_content_len = 0;
		return SC_SUCCESS;
	}

	tmp = (u8 *) realloc(file->encoded_content, content_len);
	if (!tmp) {
		if (file->encoded_content)
			free(file->encoded_content);
		file->encoded_content = NULL;
		file->encoded_content_len = 0;
		return SC_ERROR_OUT_OF_MEMORY;
	}

	file->encoded_content = tmp;
	memcpy(file->encoded_content, content, content_len);
	file->encoded_content_len = content_len;

	return SC_SUCCESS;
}