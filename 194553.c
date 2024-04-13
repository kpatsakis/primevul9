static void qxl_rom_set_dirty(PCIQXLDevice *qxl)
{
    qxl_set_dirty(&qxl->rom_bar, 0, qxl->rom_size);
}