static int interface_get_command(QXLInstance *sin, struct QXLCommandExt *ext)
{
    PCIQXLDevice *qxl = container_of(sin, PCIQXLDevice, ssd.qxl);
    SimpleSpiceUpdate *update;
    QXLCommandRing *ring;
    QXLCommand *cmd;
    int notify, ret;

    trace_qxl_ring_command_check(qxl->id, qxl_mode_to_string(qxl->mode));

    switch (qxl->mode) {
    case QXL_MODE_VGA:
        ret = false;
        qemu_mutex_lock(&qxl->ssd.lock);
        update = QTAILQ_FIRST(&qxl->ssd.updates);
        if (update != NULL) {
            QTAILQ_REMOVE(&qxl->ssd.updates, update, next);
            *ext = update->ext;
            ret = true;
        }
        qemu_mutex_unlock(&qxl->ssd.lock);
        if (ret) {
            trace_qxl_ring_command_get(qxl->id, qxl_mode_to_string(qxl->mode));
            qxl_log_command(qxl, "vga", ext);
        }
        return ret;
    case QXL_MODE_COMPAT:
    case QXL_MODE_NATIVE:
    case QXL_MODE_UNDEFINED:
        ring = &qxl->ram->cmd_ring;
        if (qxl->guest_bug || SPICE_RING_IS_EMPTY(ring)) {
            return false;
        }
        SPICE_RING_CONS_ITEM(qxl, ring, cmd);
        if (!cmd) {
            return false;
        }
        ext->cmd      = *cmd;
        ext->group_id = MEMSLOT_GROUP_GUEST;
        ext->flags    = qxl->cmdflags;
        SPICE_RING_POP(ring, notify);
        qxl_ring_set_dirty(qxl);
        if (notify) {
            qxl_send_events(qxl, QXL_INTERRUPT_DISPLAY);
        }
        qxl->guest_primary.commands++;
        qxl_track_command(qxl, ext);
        qxl_log_command(qxl, "cmd", ext);
        {
            /*
             * Windows 8 drivers place qxl commands in the vram
             * (instead of the ram) bar.  We can't live migrate such a
             * guest, so add a migration blocker in case we detect
             * this, to avoid triggering the assert in pre_save().
             *
             * https://cgit.freedesktop.org/spice/win32/qxl-wddm-dod/commit/?id=f6e099db39e7d0787f294d5fd0dce328b5210faa
             */
            void *msg = qxl_phys2virt(qxl, ext->cmd.data, ext->group_id);
            if (msg != NULL && (
                    msg < (void *)qxl->vga.vram_ptr ||
                    msg > ((void *)qxl->vga.vram_ptr + qxl->vga.vram_size))) {
                if (!qxl->migration_blocker) {
                    Error *local_err = NULL;
                    error_setg(&qxl->migration_blocker,
                               "qxl: guest bug: command not in ram bar");
                    migrate_add_blocker(qxl->migration_blocker, &local_err);
                    if (local_err) {
                        error_report_err(local_err);
                    }
                }
            }
        }
        trace_qxl_ring_command_get(qxl->id, qxl_mode_to_string(qxl->mode));
        return true;
    default:
        return false;
    }
}