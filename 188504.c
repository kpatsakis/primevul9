static void build_guest_fsinfo_for_virtual_device(char const *syspath,
                                                  GuestFilesystemInfo *fs,
                                                  Error **errp)
{
    DIR *dir;
    char *dirpath;
    struct dirent *entry;

    dirpath = g_strdup_printf("%s/slaves", syspath);
    dir = opendir(dirpath);
    if (!dir) {
        if (errno != ENOENT) {
            error_setg_errno(errp, errno, "opendir(\"%s\")", dirpath);
        }
        g_free(dirpath);
        return;
    }

    for (;;) {
        errno = 0;
        entry = readdir(dir);
        if (entry == NULL) {
            if (errno) {
                error_setg_errno(errp, errno, "readdir(\"%s\")", dirpath);
            }
            break;
        }

        if (entry->d_type == DT_LNK) {
            char *path;

            g_debug(" slave device '%s'", entry->d_name);
            path = g_strdup_printf("%s/slaves/%s", syspath, entry->d_name);
            build_guest_fsinfo_for_device(path, fs, errp);
            g_free(path);

            if (*errp) {
                break;
            }
        }
    }

    g_free(dirpath);
    closedir(dir);
}