_zip_cdir_new(zip_uint64_t nentry, zip_error_t *error)
{
    zip_cdir_t *cd;

    if ((cd=(zip_cdir_t *)malloc(sizeof(*cd))) == NULL) {
	zip_error_set(error, ZIP_ER_MEMORY, 0);
	return NULL;
    }

    cd->entry = NULL;
    cd->nentry = cd->nentry_alloc = 0;
    cd->size = cd->offset = 0;
    cd->comment = NULL;
    cd->is_zip64 = false;

    if (!_zip_cdir_grow(cd, nentry, error)) {
	_zip_cdir_free(cd);
	return NULL;
    }

    return cd;
}