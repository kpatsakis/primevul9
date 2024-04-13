int sc_file_valid(const sc_file_t *file) {
	if (file == NULL)
		return 0;
	return file->magic == SC_FILE_MAGIC;
}