static void qxl_spice_destroy_surface_wait_complete(PCIQXLDevice *qxl,
                                                    uint32_t id)
{
    trace_qxl_spice_destroy_surface_wait_complete(qxl->id, id);
    qemu_mutex_lock(&qxl->track_lock);
    qxl->guest_surfaces.cmds[id] = 0;
    qxl->guest_surfaces.count--;
    qemu_mutex_unlock(&qxl->track_lock);
}