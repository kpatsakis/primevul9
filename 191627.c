_zip_dirent_finalize(zip_dirent_t *zde)
{
    if (!zde->cloned || zde->changed & ZIP_DIRENT_FILENAME) {
	_zip_string_free(zde->filename);
	zde->filename = NULL;
    }
    if (!zde->cloned || zde->changed & ZIP_DIRENT_EXTRA_FIELD) {
	_zip_ef_free(zde->extra_fields);
	zde->extra_fields = NULL;
    }
    if (!zde->cloned || zde->changed & ZIP_DIRENT_COMMENT) {
	_zip_string_free(zde->comment);
	zde->comment = NULL;
    }
}