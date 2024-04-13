zzip_disk_init(ZZIP_DISK* disk, char* buffer, zzip_size_t buflen)
{
    disk->buffer = buffer;
    disk->endbuf = buffer+buflen;
    disk->reserved = 0;
    disk->flags = 0;
    disk->mapped = 0;
    /* do not touch disk->user */
    /* do not touch disk->code */
    return 0;
}