void qxl_spice_reset_cursor(PCIQXLDevice *qxl)
{
    trace_qxl_spice_reset_cursor(qxl->id);
    spice_qxl_reset_cursor(&qxl->ssd.qxl);
    qemu_mutex_lock(&qxl->track_lock);
    qxl->guest_cursor = 0;
    qemu_mutex_unlock(&qxl->track_lock);
    if (qxl->ssd.cursor) {
        cursor_put(qxl->ssd.cursor);
    }
    qxl->ssd.cursor = cursor_builtin_hidden();
}