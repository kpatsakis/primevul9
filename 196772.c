zzip_disk_feof (ZZIP_DISK_FILE* file)
{
    return ! file || ! file->avail;
}