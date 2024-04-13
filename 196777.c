zzip_mem_disk_close(ZZIP_MEM_DISK* _zzip_restrict dir) 
{
    if (dir) {
	zzip_mem_disk_unload (dir);
	zzip_disk_close(dir->disk);
	free (dir);
    }
}