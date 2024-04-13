static void qxl_exit_vga_mode(PCIQXLDevice *d)
{
    if (d->mode != QXL_MODE_VGA) {
        return;
    }
    trace_qxl_exit_vga_mode(d->id);
    graphic_console_set_hwops(d->ssd.dcl.con, &qxl_ops, d);
    update_displaychangelistener(&d->ssd.dcl, GUI_REFRESH_INTERVAL_IDLE);
    vga_dirty_log_stop(&d->vga);
    qxl_destroy_primary(d, QXL_SYNC);
}