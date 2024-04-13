static void qxl_ring_set_dirty(PCIQXLDevice *qxl)
{
    ram_addr_t addr = qxl->shadow_rom.ram_header_offset;
    ram_addr_t end  = qxl->vga.vram_size;
    qxl_set_dirty(&qxl->vga.vram, addr, end);
}