zzip_disk_findmatch(ZZIP_DISK* disk, char* filespec, 
		    struct zzip_disk_entry* after,
		    zzip_fnmatch_fn_t compare, int flags)
{
    struct zzip_disk_entry* entry = (! after ? zzip_disk_findfirst (disk) 
				     : zzip_disk_findnext (disk, after));
    if (! compare) { 
	compare = (zzip_fnmatch_fn_t) _zzip_fnmatch; 
	if (disk->flags&1) disk->flags |= _zzip_fnmatch_CASEFOLD;
    }
    for (; entry ; entry = zzip_disk_findnext (disk, entry))
    {
	/* filenames within zip files are often not null-terminated! */
	char* realname = zzip_disk_entry_strdup_name(disk, entry);
	if (realname && ! compare(filespec, realname, flags))
	{
	    free (realname);
	    return entry;
	}
	free (realname);
    }
    return 0;
}