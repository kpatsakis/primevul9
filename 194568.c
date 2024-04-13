static void qxl_soft_reset(PCIQXLDevice *d)
{
    trace_qxl_soft_reset(d->id);
    qxl_check_state(d);
    qxl_clear_guest_bug(d);
    qemu_mutex_lock(&d->async_lock);
    d->current_async = QXL_UNDEFINED_IO;
    qemu_mutex_unlock(&d->async_lock);

    if (d->have_vga) {
        qxl_enter_vga_mode(d);
    } else {
        d->mode = QXL_MODE_UNDEFINED;
    }
}