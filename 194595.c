static void qxl_enter_vga_mode(PCIQXLDevice *d)
{
    if (d->mode == QXL_MODE_VGA) {
        return;
    }
    trace_qxl_enter_vga_mode(d->id);
    spice_qxl_driver_unload(&d->ssd.qxl);
    graphic_console_set_hwops(d->ssd.dcl.con, d->vga.hw_ops, &d->vga);
    update_displaychangelistener(&d->ssd.dcl, GUI_REFRESH_INTERVAL_DEFAULT);
    qemu_spice_create_host_primary(&d->ssd);
    d->mode = QXL_MODE_VGA;
    qemu_spice_display_switch(&d->ssd, d->ssd.ds);
    vga_dirty_log_start(&d->vga);
    graphic_hw_update(d->vga.con);
}