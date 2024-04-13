static int interface_get_cursor_command(QXLInstance *sin, struct QXLCommandExt *ext)
{
    PCIQXLDevice *qxl = container_of(sin, PCIQXLDevice, ssd.qxl);
    QXLCursorRing *ring;
    QXLCommand *cmd;
    int notify;

    trace_qxl_ring_cursor_check(qxl->id, qxl_mode_to_string(qxl->mode));

    switch (qxl->mode) {
    case QXL_MODE_COMPAT:
    case QXL_MODE_NATIVE:
    case QXL_MODE_UNDEFINED:
        ring = &qxl->ram->cursor_ring;
        if (SPICE_RING_IS_EMPTY(ring)) {
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
            qxl_send_events(qxl, QXL_INTERRUPT_CURSOR);
        }
        qxl->guest_primary.commands++;
        qxl_track_command(qxl, ext);
        qxl_log_command(qxl, "csr", ext);
        if (qxl->have_vga) {
            qxl_render_cursor(qxl, ext);
        }
        trace_qxl_ring_cursor_get(qxl->id, qxl_mode_to_string(qxl->mode));
        return true;
    default:
        return false;
    }
}