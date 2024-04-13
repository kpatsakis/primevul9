static void qxl_spice_flush_surfaces_async(PCIQXLDevice *qxl)
{
    trace_qxl_spice_flush_surfaces_async(qxl->id, qxl->guest_surfaces.count,
                                         qxl->num_free_res);
    spice_qxl_flush_surfaces_async(&qxl->ssd.qxl,
        (uintptr_t)qxl_cookie_new(QXL_COOKIE_TYPE_IO,
                                  QXL_IO_FLUSH_SURFACES_ASYNC));
}