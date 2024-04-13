RAMBlock *qemu_ram_alloc(struct uc_struct *uc, ram_addr_t size, MemoryRegion *mr)
{
    return qemu_ram_alloc_from_ptr(uc, size, NULL, mr);
}