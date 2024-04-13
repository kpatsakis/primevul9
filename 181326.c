static uint32_t stat_to_v9mode(const struct stat *stbuf)
{
    uint32_t mode;

    mode = stbuf->st_mode & 0777;
    if (S_ISDIR(stbuf->st_mode)) {
        mode |= P9_STAT_MODE_DIR;
    }

    if (S_ISLNK(stbuf->st_mode)) {
        mode |= P9_STAT_MODE_SYMLINK;
    }

    if (S_ISSOCK(stbuf->st_mode)) {
        mode |= P9_STAT_MODE_SOCKET;
    }

    if (S_ISFIFO(stbuf->st_mode)) {
        mode |= P9_STAT_MODE_NAMED_PIPE;
    }

    if (S_ISBLK(stbuf->st_mode) || S_ISCHR(stbuf->st_mode)) {
        mode |= P9_STAT_MODE_DEVICE;
    }

    if (stbuf->st_mode & S_ISUID) {
        mode |= P9_STAT_MODE_SETUID;
    }

    if (stbuf->st_mode & S_ISGID) {
        mode |= P9_STAT_MODE_SETGID;
    }

    if (stbuf->st_mode & S_ISVTX) {
        mode |= P9_STAT_MODE_SETVTX;
    }

    return mode;
}