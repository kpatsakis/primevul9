zzip_disk_entry_to_data(ZZIP_DISK* disk, struct zzip_disk_entry* entry)
{
    struct zzip_file_header* file = 
	zzip_disk_entry_to_file_header(disk, entry);
    if (file) return zzip_file_header_to_data (file);
    return 0;
}