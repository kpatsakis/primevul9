zzip_disk_fclose (ZZIP_DISK_FILE* file)
{
    if (! file->stored)
	inflateEnd (& file->zlib);
    free (file);
    return 0;
}