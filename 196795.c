zzip_disk_entry_to_file_header(ZZIP_DISK* disk, struct zzip_disk_entry* entry)
{
    char* file_header = /* (struct zzip_file_header*) */
	(disk->buffer + zzip_disk_entry_fileoffset (entry));
    if (disk->buffer > file_header || file_header >= disk->endbuf) 
	return 0;
    return (struct zzip_file_header*) file_header;
}