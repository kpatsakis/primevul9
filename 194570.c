static void interface_set_compression_level(QXLInstance *sin, int level)
{
    PCIQXLDevice *qxl = container_of(sin, PCIQXLDevice, ssd.qxl);

    trace_qxl_interface_set_compression_level(qxl->id, level);
    qxl->shadow_rom.compression_level = cpu_to_le32(level);
    qxl->rom->compression_level = cpu_to_le32(level);
    qxl_rom_set_dirty(qxl);
}