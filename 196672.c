static int create_or_die(const char *filename)
{
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC | O_EXCL, DEFAULT_DUMP_DIR_MODE);
    if (fd >= 0)
    {
        IGNORE_RESULT(fchown(fd, dd->dd_uid, dd->dd_gid));
        return fd;
    }

    int sv_errno = errno;
    if (dd)
        dd_delete(dd);
    if (user_core_fd >= 0)
        unlinkat(dirfd(proc_cwd), core_basename, /*unlink file*/0);

    errno = sv_errno;
    perror_msg_and_die("Can't open '%s'", filename);
}