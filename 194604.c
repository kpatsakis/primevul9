static void qxl_ram_set_dirty(PCIQXLDevice *qxl, void *ptr)
{
    void *base = qxl->vga.vram_ptr;
    intptr_t offset;

    offset = ptr - base;
    assert(offset < qxl->vga.vram_size);
    qxl_set_dirty(&qxl->vga.vram, offset, offset + 3);
}