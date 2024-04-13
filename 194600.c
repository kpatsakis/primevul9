void qxl_spice_oom(PCIQXLDevice *qxl)
{
    trace_qxl_spice_oom(qxl->id);
    spice_qxl_oom(&qxl->ssd.qxl);
}