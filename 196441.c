zzip_mem_disk_load(ZZIP_MEM_DISK * dir, ZZIP_DISK * disk)
{
    if (! dir || ! disk)
        { errno=EINVAL; return -1; }
    if (dir->list)
        zzip_mem_disk_unload(dir);
    ___ long count = 0;
    ___ struct zzip_disk_entry *entry = zzip_disk_findfirst(disk);
    if (!entry) goto error;
    for (; entry; entry = zzip_disk_findnext(disk, entry))
    {
        ZZIP_MEM_ENTRY *item = zzip_mem_entry_new(disk, entry);
        if (! item)
        {
            debug1("unable to load entry");
            goto error;
        }
        if (dir->last)
        {
            dir->last->zz_next = item;  /* chain last */
        } else
        {
            dir->list = item;
        }
        dir->last = item;       /* to earlier */
        count++;
    }
    ____;
    dir->disk = disk;
    return count;
    ____;
  error:
    zzip_mem_disk_unload(dir);
    return -1;
}