void sc_file_free(sc_file_t *file)
{
	unsigned int i;
	if (file == NULL || !sc_file_valid(file))
		return;
	file->magic = 0;
	for (i = 0; i < SC_MAX_AC_OPS; i++)
		sc_file_clear_acl_entries(file, i);
	if (file->sec_attr)
		free(file->sec_attr);
	if (file->prop_attr)
		free(file->prop_attr);
	if (file->type_attr)
		free(file->type_attr);
	if (file->encoded_content)
		free(file->encoded_content);
	free(file);
}