static void qxl_vga_ioport_write(void *opaque, uint32_t addr, uint32_t val)
{
    VGACommonState *vga = opaque;
    PCIQXLDevice *qxl = container_of(vga, PCIQXLDevice, vga);

    trace_qxl_io_write_vga(qxl->id, qxl_mode_to_string(qxl->mode), addr, val);
    if (qxl->mode != QXL_MODE_VGA) {
        qxl_destroy_primary(qxl, QXL_SYNC);
        qxl_soft_reset(qxl);
    }
    vga_ioport_write(opaque, addr, val);
}