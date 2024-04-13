zzip_mem_disk_fopen (ZZIP_MEM_DISK* dir, char* filename) 
{
    ZZIP_MEM_ENTRY* entry = zzip_mem_disk_findfile (dir, filename, 0, 0);
    if (! entry) return 0; else return zzip_mem_entry_fopen (dir, entry);
}