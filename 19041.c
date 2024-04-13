dnsc_read_from_file(char *fname, char *buf, size_t count)
{
    int fd;
    fd = open(fname, O_RDONLY);
    if (fd == -1) {
        return -1;
    }
    if (read(fd, buf, count) != (ssize_t)count) {
        close(fd);
        return -2;
    }
    close(fd);
    return 0;
}