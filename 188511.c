GuestFilesystemInfoList *qmp_guest_get_fsinfo(Error **errp)
{
    HANDLE vol_h;
    GuestFilesystemInfoList *new, *ret = NULL;
    char guid[256];

    vol_h = FindFirstVolume(guid, sizeof(guid));
    if (vol_h == INVALID_HANDLE_VALUE) {
        error_setg_win32(errp, GetLastError(), "failed to find any volume");
        return NULL;
    }

    do {
        GuestFilesystemInfo *info = build_guest_fsinfo(guid, errp);
        if (info == NULL) {
            continue;
        }
        new = g_malloc(sizeof(*ret));
        new->value = info;
        new->next = ret;
        ret = new;
    } while (FindNextVolume(vol_h, guid, sizeof(guid)));

    if (GetLastError() != ERROR_NO_MORE_FILES) {
        error_setg_win32(errp, GetLastError(), "failed to find next volume");
    }

    FindVolumeClose(vol_h);
    return ret;
}