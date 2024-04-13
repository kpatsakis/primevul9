zzip_disk_findnext(ZZIP_DISK* disk, struct zzip_disk_entry* entry)
{
    if ((char*)entry < disk->buffer || 
	(char*)entry > disk->endbuf-sizeof(entry) ||
	! zzip_disk_entry_check_magic (entry) ||
	zzip_disk_entry_sizeto_end (entry) > 64*1024)
	return 0;
    entry = zzip_disk_entry_to_next_entry (entry);
    if ((char*)entry > disk->endbuf-sizeof(entry) ||
	! zzip_disk_entry_check_magic (entry) ||
	zzip_disk_entry_sizeto_end (entry) > 64*1024 ||
	zzip_disk_entry_skipto_end (entry) + sizeof(entry) > disk->endbuf)
	return 0;
    else
	return entry;
}