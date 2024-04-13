zzip_disk_new(void)
{
    ZZIP_DISK* disk = malloc(sizeof(disk));
    if (! disk) return disk;
    zzip_disk_init (disk, 0, 0);
    return disk;
}