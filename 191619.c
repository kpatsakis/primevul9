_zip_dirent_free(zip_dirent_t *zde)
{
    if (zde == NULL)
	return;

    _zip_dirent_finalize(zde);
    free(zde);
}