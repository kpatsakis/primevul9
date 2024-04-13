zzip_mem_disk_unload(ZZIP_MEM_DISK* dir)
{
    ZZIP_MEM_ENTRY* item = dir->list;
    while (item) {
	ZZIP_MEM_ENTRY* next = item->zz_next;
	zzip_mem_entry_free(item); item = next;
    }
    dir->list = dir->last = 0; dir->disk = 0;
}