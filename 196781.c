zzip_disk_findfile(ZZIP_DISK* disk, char* filename, 
		    struct zzip_disk_entry* after, zzip_strcmp_fn_t compare)
{
    struct zzip_disk_entry* entry = (! after ? zzip_disk_findfirst (disk) 
				     : zzip_disk_findnext (disk, after));
    if (! compare) 
	compare = (zzip_strcmp_fn_t)( (disk->flags&1) ? 
				      (_zzip_strcasecmp) : (strcmp));
    for (; entry ; entry = zzip_disk_findnext (disk, entry))
    {
	/* filenames within zip files are often not null-terminated! */
	char* realname = zzip_disk_entry_strdup_name (disk, entry);
	if (realname && ! compare(filename, realname))
	{
	    free (realname);
	    return entry;
	}
	free (realname);
    }
    return 0;
}