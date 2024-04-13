void qxl_spice_reset_memslots(PCIQXLDevice *qxl)
{
    trace_qxl_spice_reset_memslots(qxl->id);
    spice_qxl_reset_memslots(&qxl->ssd.qxl);
}