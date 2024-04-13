static int qxl_destroy_primary(PCIQXLDevice *d, qxl_async_io async)
{
    if (d->mode == QXL_MODE_UNDEFINED) {
        return 0;
    }
    trace_qxl_destroy_primary(d->id);
    d->mode = QXL_MODE_UNDEFINED;
    qemu_spice_destroy_primary_surface(&d->ssd, 0, async);
    qxl_spice_reset_cursor(d);
    return 1;
}