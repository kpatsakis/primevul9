static errno_t safe_remove_old_ccache_file(const char *old_ccache_file,
                                           const char *new_ccache_file)
{
    int ret;
    size_t old_offset = 0;
    size_t new_offset = 0;

    if (new_ccache_file == NULL) {
        DEBUG(1, ("Missing new ccache file, "
                  "old ccache file is not deleted.\n"));
        return EINVAL;
    }

    if (old_ccache_file != NULL) {
        if (strncmp(old_ccache_file, "FILE:", 5) == 0) {
            old_offset = 5;
        }
        if (strncmp(new_ccache_file, "FILE:", 5) == 0) {
            new_offset = 5;
        }
        if (strcmp(old_ccache_file + old_offset,
                   new_ccache_file + new_offset) == 0) {
            DEBUG(7, ("New and old ccache file are the same, "
                      "no one will be deleted.\n"));
            return EOK;
        }
        if (old_ccache_file[old_offset] != '/') {
            DEBUG(1, ("Ccache file name [%s] is not an absolute path.\n",
                      old_ccache_file + old_offset));
            return EINVAL;
        }
        ret = unlink(old_ccache_file + old_offset);
        if (ret == -1 && errno != ENOENT) {
            ret = errno;
            DEBUG(1, ("unlink [%s] failed [%d][%s].\n", old_ccache_file, ret,
                                                        strerror(ret)));
            return ret;
        }
    }

    return EOK;
}