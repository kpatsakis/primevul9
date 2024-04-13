static void qxl_set_dirty(MemoryRegion *mr, ram_addr_t addr, ram_addr_t end)
{
    memory_region_set_dirty(mr, addr, end - addr);
}