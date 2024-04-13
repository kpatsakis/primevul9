zzip_disk_entry_strdup_comment(ZZIP_DISK* disk, struct zzip_disk_entry* entry)
{
    if (! disk || ! entry) return 0;

    ___ char* text; zzip_size_t len;
    if ((len = zzip_disk_entry_comment (entry)))
	text = zzip_disk_entry_to_comment (entry);
    else
	return 0;

    if (disk->buffer > text || text+len > disk->endbuf)
	return 0;
    
    return  _zzip_strndup (text, len); ____;
}