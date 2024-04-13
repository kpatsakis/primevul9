__zzip_dir_parse(ZZIP_DIR * dir)
{
    zzip_error_t rv;
    zzip_off_t filesize;
    struct _disk_trailer trailer;
    /* if (! dir || dir->fd < 0)
     *     { rv = EINVAL; goto error; }
     */

    HINT2("------------------ fd=%i", (int) dir->fd);
    if ((filesize = dir->io->fd.filesize(dir->fd)) < 0)
        { rv = ZZIP_DIR_STAT; goto error; }

    HINT2("------------------ filesize=%ld", (long) filesize);
    if ((rv = __zzip_fetch_disk_trailer(dir->fd, filesize, &trailer,
                                        dir->io)) != 0)
        { goto error; }

    HINT5("directory = { entries= %ld/%ld, size= %ld, seek= %ld } ",
          (long) _disk_trailer_localentries(&trailer),
          (long) _disk_trailer_finalentries(&trailer),
          (long) _disk_trailer_rootsize(&trailer),
          (long) _disk_trailer_rootseek(&trailer));

    if ((rv = __zzip_parse_root_directory(dir->fd, &trailer, &dir->hdr0,
                                          dir->io)) != 0)
        { goto error; }
  error:
    return rv;
}