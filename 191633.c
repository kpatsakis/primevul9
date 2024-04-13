_zip_dirent_new(void)
{
    zip_dirent_t *de;

    if ((de=(zip_dirent_t *)malloc(sizeof(*de))) == NULL)
	return NULL;

    _zip_dirent_init(de);
    return de;
}