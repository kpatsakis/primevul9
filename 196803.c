zzip_disk_fopen (ZZIP_DISK* disk, char* filename)
{
    ZZIP_DISK_ENTRY* entry = zzip_disk_findfile (disk, filename, 0, 0);
    if (! entry) return 0; else return zzip_disk_entry_fopen (disk, entry);
}