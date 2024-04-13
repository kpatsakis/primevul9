static GuestFilesystemInfo *build_guest_fsinfo(struct FsMount *mount,
                                               Error **errp)
{
    GuestFilesystemInfo *fs = g_malloc0(sizeof(*fs));
    char *devpath = g_strdup_printf("/sys/dev/block/%u:%u",
                                    mount->devmajor, mount->devminor);

    fs->mountpoint = g_strdup(mount->dirname);
    fs->type = g_strdup(mount->devtype);
    build_guest_fsinfo_for_device(devpath, fs, errp);

    g_free(devpath);
    return fs;
}