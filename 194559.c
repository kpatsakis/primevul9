static void qxl_reset_surfaces(PCIQXLDevice *d)
{
    trace_qxl_reset_surfaces(d->id);
    d->mode = QXL_MODE_UNDEFINED;
    qxl_spice_destroy_surfaces(d, QXL_SYNC);
}