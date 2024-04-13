void qxl_spice_loadvm_commands(PCIQXLDevice *qxl, struct QXLCommandExt *ext,
                               uint32_t count)
{
    trace_qxl_spice_loadvm_commands(qxl->id, ext, count);
    spice_qxl_loadvm_commands(&qxl->ssd.qxl, ext, count);
}