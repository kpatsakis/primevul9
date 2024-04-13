static errno_t check_if_ccache_file_is_used(uid_t uid, const char *ccname,
                                            bool *result)
{
    int ret;
    size_t offset = 0;
    struct stat stat_buf;
    const char *filename;
    bool active;

    *result = false;

    if (ccname == NULL || *ccname == '\0') {
        return EINVAL;
    }

    if (strncmp(ccname, "FILE:", 5) == 0) {
        offset = 5;
    }

    filename = ccname + offset;

    if (filename[0] != '/') {
        DEBUG(1, ("Only absolute path names are allowed.\n"));
        return EINVAL;
    }

    ret = lstat(filename, &stat_buf);

    if (ret == -1 && errno != ENOENT) {
        DEBUG(1, ("stat failed [%d][%s].\n", errno, strerror(errno)));
        return errno;
    } else if (ret == EOK) {
        if (stat_buf.st_uid != uid) {
            DEBUG(1, ("Cache file [%s] exists, but is owned by [%d] instead of "
                      "[%d].\n", filename, stat_buf.st_uid, uid));
            return EINVAL;
        }

        if (!S_ISREG(stat_buf.st_mode)) {
            DEBUG(1, ("Cache file [%s] exists, but is not a regular file.\n",
                      filename));
            return EINVAL;
        }
    }

    ret = check_if_uid_is_active(uid, &active);
    if (ret != EOK) {
        DEBUG(1, ("check_if_uid_is_active failed.\n"));
        return ret;
    }

    if (!active) {
        DEBUG(5, ("User [%d] is not active\n", uid));
    } else {
        DEBUG(9, ("User [%d] is still active, reusing ccache file [%s].\n",
                  uid, filename));
        *result = true;
    }
    return EOK;
}