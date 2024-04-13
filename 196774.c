zzip_disk_entry_strdup_name(ZZIP_DISK* disk, struct zzip_disk_entry* entry)
{
    if (! disk || ! entry) return 0;

    ___ char* name; zzip_size_t len;
    struct zzip_file_header* file;
    if ((len = zzip_disk_entry_namlen (entry)))
	name = zzip_disk_entry_to_filename (entry);
    else if ((file = zzip_disk_entry_to_file_header (disk, entry)) &&
	     (len = zzip_file_header_namlen (file)))
	name = zzip_file_header_to_filename (file);
    else
	return 0;

    if (disk->buffer > name || name+len > disk->endbuf)
	return 0;
    
    return  _zzip_strndup (name, len); ____;
}