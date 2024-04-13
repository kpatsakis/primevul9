static void ga_read_sysfs_file(int dirfd, const char *pathname, char *buf,
                               int size, Error **errp)
{
    int fd;
    int res;

    errno = 0;
    fd = openat(dirfd, pathname, O_RDONLY);
    if (fd == -1) {
        error_setg_errno(errp, errno, "open sysfs file \"%s\"", pathname);
        return;
    }

    res = pread(fd, buf, size, 0);
    if (res == -1) {
        error_setg_errno(errp, errno, "pread sysfs file \"%s\"", pathname);
    } else if (res == 0) {
        error_setg(errp, "pread sysfs file \"%s\": unexpected EOF", pathname);
    }
    close(fd);
}