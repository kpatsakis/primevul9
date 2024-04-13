_zip_get_dirent(zip_t *za, zip_uint64_t idx, zip_flags_t flags, zip_error_t *error)
{
    if (error == NULL)
	error = &za->error;

    if (idx >= za->nentry) {
	zip_error_set(error, ZIP_ER_INVAL, 0);
	return NULL;
    }

    if ((flags & ZIP_FL_UNCHANGED) || za->entry[idx].changes == NULL) {
	if (za->entry[idx].orig == NULL) {
	    zip_error_set(error, ZIP_ER_INVAL, 0);
	    return NULL;
	}
	if (za->entry[idx].deleted && (flags & ZIP_FL_UNCHANGED) == 0) {
	    zip_error_set(error, ZIP_ER_DELETED, 0);
	    return NULL;
	}
	return za->entry[idx].orig;
    }
    else
	return za->entry[idx].changes;
}