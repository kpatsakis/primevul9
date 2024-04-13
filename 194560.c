static void interface_set_mm_time(QXLInstance *sin, uint32_t mm_time)
{
    PCIQXLDevice *qxl = container_of(sin, PCIQXLDevice, ssd.qxl);

    if (!qemu_spice_display_is_running(&qxl->ssd)) {
        return;
    }

    trace_qxl_interface_set_mm_time(qxl->id, mm_time);
    qxl->shadow_rom.mm_clock = cpu_to_le32(mm_time);
    qxl->rom->mm_clock = cpu_to_le32(mm_time);
    qxl_rom_set_dirty(qxl);
}