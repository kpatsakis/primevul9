static void qxl_spice_destroy_surfaces_complete(PCIQXLDevice *qxl)
{
    trace_qxl_spice_destroy_surfaces_complete(qxl->id);
    qemu_mutex_lock(&qxl->track_lock);
    memset(qxl->guest_surfaces.cmds, 0,
           sizeof(qxl->guest_surfaces.cmds[0]) * qxl->ssd.num_surfaces);
    qxl->guest_surfaces.count = 0;
    qemu_mutex_unlock(&qxl->track_lock);
}