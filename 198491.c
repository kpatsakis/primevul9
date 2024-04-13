sc_file_t * sc_file_new(void)
{
	sc_file_t *file = (sc_file_t *)calloc(1, sizeof(sc_file_t));
	if (file == NULL)
		return NULL;

	file->magic = SC_FILE_MAGIC;
	return file;
}