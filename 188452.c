static void ga_write_sysfs_file(int dirfd, const char *pathname,
                                const char *buf, int size, Error **errp)
{
    int fd;

    errno = 0;
    fd = openat(dirfd, pathname, O_WRONLY);
    if (fd == -1) {
        error_setg_errno(errp, errno, "open sysfs file \"%s\"", pathname);
        return;
    }

    if (pwrite(fd, buf, size, 0) == -1) {
        error_setg_errno(errp, errno, "pwrite sysfs file \"%s\"", pathname);
    }

    close(fd);
}