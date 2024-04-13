MemoryRegion *memory_map_io(struct uc_struct *uc, ram_addr_t begin, size_t size,
                            uc_cb_mmio_read_t read_cb, uc_cb_mmio_write_t write_cb,
                            void *user_data_read, void *user_data_write)
{
    MemoryRegion *mmio = g_new(MemoryRegion, 1);
    mmio_cbs* opaques = g_new(mmio_cbs, 1);
    MemoryRegionOps *ops = &opaques->ops;
    opaques->read = read_cb;
    opaques->write = write_cb;
    opaques->user_data_read = user_data_read;
    opaques->user_data_write = user_data_write;

    memset(ops, 0, sizeof(*ops));

    ops->read = mmio_read_wrapper;
    ops->write = mmio_write_wrapper;
    ops->endianness = DEVICE_NATIVE_ENDIAN;

    memory_region_init_io(uc, mmio, ops, opaques, size);

    mmio->destructor = mmio_region_destructor_uc;

    mmio->perms = 0;

    if (read_cb)
        mmio->perms |= UC_PROT_READ;

    if (write_cb)
        mmio->perms |= UC_PROT_WRITE;

    memory_region_add_subregion(uc->system_memory, begin, mmio);

    if (uc->cpu)
        tlb_flush(uc->cpu);

    return mmio;
}