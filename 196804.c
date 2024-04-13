zzip_disk_findfirst(ZZIP_DISK* disk)
{
    if (disk->buffer > disk->endbuf-sizeof(struct zzip_disk_trailer))
	return 0;
    ___ char* p = disk->endbuf-sizeof(struct zzip_disk_trailer);
    for (; p >= disk->buffer ; p--)
    {
	char* root; /* (struct zzip_disk_entry*) */
	if (zzip_disk_trailer_check_magic(p)) {
	    root =  disk->buffer + zzip_disk_trailer_get_rootseek (
		(struct zzip_disk_trailer*)p);
	    if (root > p) 
	    {   /* the first disk_entry is after the disk_trailer? can't be! */
		zzip_size_t rootsize = zzip_disk_trailer_get_rootsize (
		    (struct zzip_disk_trailer*)p);
		if (disk->buffer+rootsize > p) continue;
		/* a common brokeness that can be fixed: we just assume the
		 * central directory was written directly before the trailer:*/
		root = p - rootsize;
	    }
	} else if (zzip_disk64_trailer_check_magic(p)) {
	    if (sizeof(void*) < 8) return 0; /* EOVERFLOW */
	    root =  disk->buffer + zzip_disk64_trailer_get_rootseek (
		(struct zzip_disk64_trailer*)p);
	    if (root > p) continue; 
	} else continue;

	if (root < disk->buffer) continue;
	if (zzip_disk_entry_check_magic(root)) 
	    return (struct zzip_disk_entry*) root;
    }____;
    return 0;
}