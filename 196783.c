zzip_disk_mmap(int fd)
{
    struct stat st;
    if (fstat (fd, &st) || !st.st_size) return 0;
    ___ ZZIP_DISK* disk = zzip_disk_new (); if (! disk) return 0;
    disk->buffer = _zzip_mmap (& zzip->mapped, fd, 0, st.st_size);
    if (disk->buffer == MAP_FAILED) { free (disk); return 0; }
    disk->endbuf = disk->buffer + st.st_size;
    return disk; ____;
}