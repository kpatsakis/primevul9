ram_addr_t qemu_ram_alloc_resizeable(ram_addr_t size, ram_addr_t maxsz,
                                     void (*resized)(const char*,
                                                     uint64_t length,
                                                     void *host),
                                     MemoryRegion *mr, Error **errp)
{
    return qemu_ram_alloc_internal(size, maxsz, resized, NULL, true, mr, errp);
}