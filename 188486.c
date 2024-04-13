static int dev_major_minor(const char *devpath,
                           unsigned int *devmajor, unsigned int *devminor)
{
    struct stat st;

    *devmajor = 0;
    *devminor = 0;

    if (stat(devpath, &st) < 0) {
        slog("failed to stat device file '%s': %s", devpath, strerror(errno));
        return -1;
    }
    if (S_ISDIR(st.st_mode)) {
        /* It is bind mount */
        return -2;
    }
    if (S_ISBLK(st.st_mode)) {
        *devmajor = major(st.st_rdev);
        *devminor = minor(st.st_rdev);
        return 0;
    }
    return -1;
}