static void qxl_spice_destroy_surfaces(PCIQXLDevice *qxl, qxl_async_io async)
{
    trace_qxl_spice_destroy_surfaces(qxl->id, async);
    if (async) {
        spice_qxl_destroy_surfaces_async(&qxl->ssd.qxl,
                (uintptr_t)qxl_cookie_new(QXL_COOKIE_TYPE_IO,
                                          QXL_IO_DESTROY_ALL_SURFACES_ASYNC));
    } else {
        spice_qxl_destroy_surfaces(&qxl->ssd.qxl);
        qxl_spice_destroy_surfaces_complete(qxl);
    }
}