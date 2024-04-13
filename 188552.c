static int get_disk_bus_type(HANDLE vol_h, Error **errp)
{
    STORAGE_PROPERTY_QUERY query;
    STORAGE_DEVICE_DESCRIPTOR *dev_desc, buf;
    DWORD received;

    dev_desc = &buf;
    dev_desc->Size = sizeof(buf);
    query.PropertyId = StorageDeviceProperty;
    query.QueryType = PropertyStandardQuery;

    if (!DeviceIoControl(vol_h, IOCTL_STORAGE_QUERY_PROPERTY, &query,
                         sizeof(STORAGE_PROPERTY_QUERY), dev_desc,
                         dev_desc->Size, &received, NULL)) {
        error_setg_win32(errp, GetLastError(), "failed to get bus type");
        return -1;
    }

    return dev_desc->BusType;
}