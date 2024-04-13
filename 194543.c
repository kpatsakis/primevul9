static void interface_async_complete_io(PCIQXLDevice *qxl, QXLCookie *cookie)
{
    uint32_t current_async;

    qemu_mutex_lock(&qxl->async_lock);
    current_async = qxl->current_async;
    qxl->current_async = QXL_UNDEFINED_IO;
    qemu_mutex_unlock(&qxl->async_lock);

    trace_qxl_interface_async_complete_io(qxl->id, current_async, cookie);
    if (!cookie) {
        fprintf(stderr, "qxl: %s: error, cookie is NULL\n", __func__);
        return;
    }
    if (cookie && current_async != cookie->io) {
        fprintf(stderr,
                "qxl: %s: error: current_async = %d != %"
                PRId64 " = cookie->io\n", __func__, current_async, cookie->io);
    }
    switch (current_async) {
    case QXL_IO_MEMSLOT_ADD_ASYNC:
    case QXL_IO_DESTROY_PRIMARY_ASYNC:
    case QXL_IO_UPDATE_AREA_ASYNC:
    case QXL_IO_FLUSH_SURFACES_ASYNC:
    case QXL_IO_MONITORS_CONFIG_ASYNC:
        break;
    case QXL_IO_CREATE_PRIMARY_ASYNC:
        qxl_create_guest_primary_complete(qxl);
        break;
    case QXL_IO_DESTROY_ALL_SURFACES_ASYNC:
        qxl_spice_destroy_surfaces_complete(qxl);
        break;
    case QXL_IO_DESTROY_SURFACE_ASYNC:
        qxl_spice_destroy_surface_wait_complete(qxl, cookie->u.surface_id);
        break;
    default:
        fprintf(stderr, "qxl: %s: unexpected current_async %d\n", __func__,
                current_async);
    }
    qxl_send_events(qxl, QXL_INTERRUPT_IO_CMD);
}