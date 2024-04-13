zzip_disk_findfirst(ZZIP_DISK * disk)
{
    DBG1("findfirst");
    if (! disk)
    {
        DBG1("non arg");
        errno = EINVAL;
        return 0;
    }
    if (disk->buffer > disk->endbuf - sizeof(struct zzip_disk_trailer))
    {
        DBG1("not enough data for a disk trailer");
        errno = EBADMSG;
        return 0;
    }
    ___ zzip_byte_t *p = disk->endbuf - sizeof(struct zzip_disk_trailer);
    for (; p >= disk->buffer; p--)
    {
        zzip_byte_t *root;      /* (struct zzip_disk_entry*) */
	zzip_size_t rootsize;	/* Size of root central directory */

        if (zzip_disk_trailer_check_magic(p))
        {
            struct zzip_disk_trailer *trailer = (struct zzip_disk_trailer *) p;
            zzip_size_t rootseek = zzip_disk_trailer_get_rootseek(trailer);
	    rootsize = zzip_disk_trailer_get_rootsize(trailer);

            root = disk->buffer + rootseek;
            DBG2("disk rootseek at %lli", (long long)rootseek);
            if (root > p)
            {
                /* the first disk_entry is after the disk_trailer? can't be! */
                DBG2("have rootsize at %lli", (long long)rootsize);
                if (disk->buffer + rootsize > p)
                    continue;
                /* a common brokeness that can be fixed: we just assume the
                 * central directory was written directly before the trailer:*/
                root = p - rootsize;
            }
        } else if (zzip_disk64_trailer_check_magic(p))
        {
            struct zzip_disk64_trailer *trailer =
                (struct zzip_disk64_trailer *) p;
            if (sizeof(void *) < 8)
            {
                DBG1("disk64 trailer in non-largefile part");
                errno = EFBIG;
                return 0;
            }
            zzip_size_t rootseek = zzip_disk64_trailer_get_rootseek(trailer);
	    rootsize = zzip_disk64_trailer_get_rootsize(trailer);
            DBG2("disk64 rootseek at %lli", (long long)rootseek);
            root = disk->buffer + rootseek;
            if (root > p)
                continue;
        } else
        {
            continue;
        }

        DBG4("buffer %p root %p endbuf %p", disk->buffer, root, disk->endbuf);
        if (root < disk->buffer)
        {
            DBG1("root before buffer should be impossible");
            errno = EBADMSG;
            return 0;
        }
	if (root >= disk->endbuf || (root + rootsize) >= disk->endbuf)
	{
	    DBG1("root behind endbuf should be impossible");
	    errno = EBADMSG;
	    return 0;
	}
        if (zzip_disk_entry_check_magic(root))
        {
            DBG2("found the disk root %p", root);
            return (struct zzip_disk_entry *) root;
        }
    } ____;
    /* not found */
    errno = ENOENT;
    return 0;
}