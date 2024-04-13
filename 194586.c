static void init_qxl_ram(PCIQXLDevice *d)
{
    uint8_t *buf;
    uint64_t *item;

    buf = d->vga.vram_ptr;
    d->ram = (QXLRam *)(buf + le32_to_cpu(d->shadow_rom.ram_header_offset));
    d->ram->magic       = cpu_to_le32(QXL_RAM_MAGIC);
    d->ram->int_pending = cpu_to_le32(0);
    d->ram->int_mask    = cpu_to_le32(0);
    d->ram->update_surface = 0;
    d->ram->monitors_config = 0;
    SPICE_RING_INIT(&d->ram->cmd_ring);
    SPICE_RING_INIT(&d->ram->cursor_ring);
    SPICE_RING_INIT(&d->ram->release_ring);
    SPICE_RING_PROD_ITEM(d, &d->ram->release_ring, item);
    assert(item);
    *item = 0;
    qxl_ring_set_dirty(d);
}