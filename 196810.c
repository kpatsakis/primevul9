zzip_disk_munmap(ZZIP_DISK* disk)
{
    if (! disk) return 0;
    _zzip_munmap (disk->mapped, disk->buffer, disk->endbuf-disk->buffer);
    free (disk);
    return 0;
}