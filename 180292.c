_zip_cdir_grow(struct zip_cdir *cd, int nentry, struct zip_error *error)
{
    struct zip_dirent *entry;

    if (nentry < cd->nentry) {
	_zip_error_set(error, ZIP_ER_INTERNAL, 0);
	return -1;
    }

    if ((entry=((struct zip_dirent *)
		realloc(cd->entry, sizeof(*(cd->entry))*nentry))) == NULL) {
	_zip_error_set(error, ZIP_ER_MEMORY, 0);
	return -1;
    }

    cd->nentry = nentry;
    cd->entry = entry;

    return 0;
}