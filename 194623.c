static void qxl_spice_destroy_surface_wait(PCIQXLDevice *qxl, uint32_t id,
                                           qxl_async_io async)
{
    QXLCookie *cookie;

    trace_qxl_spice_destroy_surface_wait(qxl->id, id, async);
    if (async) {
        cookie = qxl_cookie_new(QXL_COOKIE_TYPE_IO,
                                QXL_IO_DESTROY_SURFACE_ASYNC);
        cookie->u.surface_id = id;
        spice_qxl_destroy_surface_async(&qxl->ssd.qxl, id, (uintptr_t)cookie);
    } else {
        spice_qxl_destroy_surface_wait(&qxl->ssd.qxl, id);
        qxl_spice_destroy_surface_wait_complete(qxl, id);
    }
}