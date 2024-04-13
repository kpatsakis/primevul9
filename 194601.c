void qxl_spice_reset_image_cache(PCIQXLDevice *qxl)
{
    trace_qxl_spice_reset_image_cache(qxl->id);
    spice_qxl_reset_image_cache(&qxl->ssd.qxl);
}