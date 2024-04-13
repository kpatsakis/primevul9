static GuestFilesystemInfo *build_guest_fsinfo(char *guid, Error **errp)
{
    DWORD info_size;
    char mnt, *mnt_point;
    char fs_name[32];
    char vol_info[MAX_PATH+1];
    size_t len;
    GuestFilesystemInfo *fs = NULL;

    GetVolumePathNamesForVolumeName(guid, (LPCH)&mnt, 0, &info_size);
    if (GetLastError() != ERROR_MORE_DATA) {
        error_setg_win32(errp, GetLastError(), "failed to get volume name");
        return NULL;
    }

    mnt_point = g_malloc(info_size + 1);
    if (!GetVolumePathNamesForVolumeName(guid, mnt_point, info_size,
                                         &info_size)) {
        error_setg_win32(errp, GetLastError(), "failed to get volume name");
        goto free;
    }

    len = strlen(mnt_point);
    mnt_point[len] = '\\';
    mnt_point[len+1] = 0;
    if (!GetVolumeInformation(mnt_point, vol_info, sizeof(vol_info), NULL, NULL,
                              NULL, (LPSTR)&fs_name, sizeof(fs_name))) {
        if (GetLastError() != ERROR_NOT_READY) {
            error_setg_win32(errp, GetLastError(), "failed to get volume info");
        }
        goto free;
    }

    fs_name[sizeof(fs_name) - 1] = 0;
    fs = g_malloc(sizeof(*fs));
    fs->name = g_strdup(guid);
    if (len == 0) {
        fs->mountpoint = g_strdup("System Reserved");
    } else {
        fs->mountpoint = g_strndup(mnt_point, len);
    }
    fs->type = g_strdup(fs_name);
    fs->disk = build_guest_disk_info(guid, errp);
free:
    g_free(mnt_point);
    return fs;
}