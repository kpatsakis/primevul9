static int interface_req_cmd_notification(QXLInstance *sin)
{
    PCIQXLDevice *qxl = container_of(sin, PCIQXLDevice, ssd.qxl);
    int wait = 1;

    trace_qxl_ring_command_req_notification(qxl->id);
    switch (qxl->mode) {
    case QXL_MODE_COMPAT:
    case QXL_MODE_NATIVE:
    case QXL_MODE_UNDEFINED:
        SPICE_RING_CONS_WAIT(&qxl->ram->cmd_ring, wait);
        qxl_ring_set_dirty(qxl);
        break;
    default:
        /* nothing */
        break;
    }
    return wait;
}