static GuestDiskAddressList *build_guest_disk_info(char *guid, Error **errp)
{
    GuestDiskAddressList *list = NULL;
    GuestDiskAddress *disk;
    SCSI_ADDRESS addr, *scsi_ad;
    DWORD len;
    int bus;
    HANDLE vol_h;

    scsi_ad = &addr;
    char *name = g_strndup(guid, strlen(guid)-1);

    vol_h = CreateFile(name, 0, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                       0, NULL);
    if (vol_h == INVALID_HANDLE_VALUE) {
        error_setg_win32(errp, GetLastError(), "failed to open volume");
        goto out_free;
    }

    bus = get_disk_bus_type(vol_h, errp);
    if (bus < 0) {
        goto out_close;
    }

    disk = g_malloc0(sizeof(*disk));
    disk->bus_type = find_bus_type(bus);
    if (bus == BusTypeScsi || bus == BusTypeAta || bus == BusTypeRAID
#if (_WIN32_WINNT >= 0x0600)
            /* This bus type is not supported before Windows Server 2003 SP1 */
            || bus == BusTypeSas
#endif
        ) {
        /* We are able to use the same ioctls for different bus types
         * according to Microsoft docs
         * https://technet.microsoft.com/en-us/library/ee851589(v=ws.10).aspx */
        if (DeviceIoControl(vol_h, IOCTL_SCSI_GET_ADDRESS, NULL, 0, scsi_ad,
                            sizeof(SCSI_ADDRESS), &len, NULL)) {
            disk->unit = addr.Lun;
            disk->target = addr.TargetId;
            disk->bus = addr.PathId;
            disk->pci_controller = get_pci_info(name, errp);
        }
        /* We do not set error in this case, because we still have enough
         * information about volume. */
    } else {
         disk->pci_controller = NULL;
    }

    list = g_malloc0(sizeof(*list));
    list->value = disk;
    list->next = NULL;
out_close:
    CloseHandle(vol_h);
out_free:
    g_free(name);
    return list;
}