_zip_cdir_grow(zip_cdir_t *cd, zip_uint64_t additional_entries, zip_error_t *error)
{
    zip_uint64_t i, new_alloc;
    zip_entry_t *new_entry;

    if (additional_entries == 0) {
	return true;
    }

    new_alloc = cd->nentry_alloc + additional_entries;

    if (new_alloc < additional_entries || new_alloc > SIZE_MAX/sizeof(*(cd->entry))) {
	zip_error_set(error, ZIP_ER_MEMORY, 0);
	return false;
    }

    if ((new_entry = (zip_entry_t *)realloc(cd->entry, sizeof(*(cd->entry))*(size_t)new_alloc)) == NULL) {
	zip_error_set(error, ZIP_ER_MEMORY, 0);
	return false;
    }

    cd->entry = new_entry;

    for (i = cd->nentry; i < new_alloc; i++) {
	_zip_entry_init(cd->entry+i);
    }

    cd->nentry = cd->nentry_alloc = new_alloc;

    return true;
}