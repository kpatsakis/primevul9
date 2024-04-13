_zip_cdir_free(struct zip_cdir *cd)
{
    int i;

    if (!cd)
	return;

    for (i=0; i<cd->nentry; i++)
	_zip_dirent_finalize(cd->entry+i);
    free(cd->comment);
    free(cd->entry);
    free(cd);
}