zzip_mem_disk_findmatch(ZZIP_MEM_DISK* dir, 
                        char* filespec, ZZIP_MEM_ENTRY* after,
			zzip_fnmatch_fn_t compare, int flags)
{
    ZZIP_MEM_ENTRY* entry = (! after ? dir->list : after->zz_next);
    if (! compare) compare = (zzip_fnmatch_fn_t) _zzip_fnmatch;
    for (; entry ; entry = entry->zz_next) {
	if (! compare (filespec, entry->zz_name, flags)) {
	    return entry;
	}
    }
    return 0;
}