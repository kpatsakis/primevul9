_zip_dirent_clone(const zip_dirent_t *sde)
{
    zip_dirent_t *tde;

    if ((tde=(zip_dirent_t *)malloc(sizeof(*tde))) == NULL)
	return NULL;

    if (sde)
	memcpy(tde, sde, sizeof(*sde));
    else
	_zip_dirent_init(tde);

    tde->changed = 0;
    tde->cloned = 1;

    return tde;
}