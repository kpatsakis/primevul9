static GuestDiskBusType find_bus_type(STORAGE_BUS_TYPE bus)
{
    if (bus > ARRAY_SIZE(win2qemu) || (int)bus < 0) {
        return GUEST_DISK_BUS_TYPE_UNKNOWN;
    }
    return win2qemu[(int)bus];
}