zzip_mem_disk_findfile(ZZIP_MEM_DISK* dir, 
                       char* filename, ZZIP_MEM_ENTRY* after,
		       zzip_strcmp_fn_t compare) 
{
    ZZIP_MEM_ENTRY* entry = (! after ? dir->list : after->zz_next);
    if (! compare) compare = (zzip_strcmp_fn_t) (strcmp);
    for (; entry ; entry = entry->zz_next) {
	if (! compare (filename, entry->zz_name)) {
	    return entry;
	}
    }
    return 0;
}