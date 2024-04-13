zzip_mem_entry_free(ZZIP_MEM_ENTRY* _zzip_restrict item) 
{
    if (item) {
	if (item->zz_ext[0]) free (item->zz_ext[0]);
	if (item->zz_ext[1]) free (item->zz_ext[1]);
	if (item->zz_ext[2]) free (item->zz_ext[2]);
	if (item->zz_comment) free (item->zz_comment);
	if (item->zz_name) free (item->zz_name);
	free (item);
    }
}