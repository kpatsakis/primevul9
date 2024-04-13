ram_addr_t qemu_ram_alloc(ram_addr_t size, MemoryRegion *mr, Error **errp)
{
    return qemu_ram_alloc_internal(size, size, NULL, NULL, false, mr, errp);
}