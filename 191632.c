_zip_cdir_free(zip_cdir_t *cd)
{
    zip_uint64_t i;

    if (!cd)
	return;

    for (i=0; i<cd->nentry; i++)
	_zip_entry_finalize(cd->entry+i);
    free(cd->entry);
    _zip_string_free(cd->comment);
    free(cd);
}