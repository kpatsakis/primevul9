zzip_disk_close(ZZIP_DISK* disk)
{
    if (! disk) return 0;
    if (disk->mapped != -1) return zzip_disk_munmap (disk);
    free (disk->buffer);
    free (disk);
    return 0;
}