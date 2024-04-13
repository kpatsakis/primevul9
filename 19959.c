void memory_region_init_ram_ptr(struct uc_struct *uc,
                                MemoryRegion *mr,
                                uint64_t size,
                                void *ptr)
{
    memory_region_init(uc, mr, size);
    mr->ram = true;
    mr->terminates = true;
    mr->destructor = memory_region_destructor_ram;

    /* qemu_ram_alloc_from_ptr cannot fail with ptr != NULL.  */
    assert(ptr != NULL);
    mr->ram_block = qemu_ram_alloc_from_ptr(uc, size, ptr, mr);
}