static void build_guest_fsinfo_for_device(char const *devpath,
                                          GuestFilesystemInfo *fs,
                                          Error **errp)
{
    char *syspath = realpath(devpath, NULL);

    if (!syspath) {
        error_setg_errno(errp, errno, "realpath(\"%s\")", devpath);
        return;
    }

    if (!fs->name) {
        fs->name = g_path_get_basename(syspath);
    }

    g_debug("  parse sysfs path '%s'", syspath);

    if (strstr(syspath, "/devices/virtual/block/")) {
        build_guest_fsinfo_for_virtual_device(syspath, fs, errp);
    } else {
        build_guest_fsinfo_for_real_device(syspath, fs, errp);
    }

    free(syspath);
}