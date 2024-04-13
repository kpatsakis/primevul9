static void interface_update_area_complete(QXLInstance *sin,
        uint32_t surface_id,
        QXLRect *dirty, uint32_t num_updated_rects)
{
    PCIQXLDevice *qxl = container_of(sin, PCIQXLDevice, ssd.qxl);
    int i;
    int qxl_i;

    qemu_mutex_lock(&qxl->ssd.lock);
    if (surface_id != 0 || !num_updated_rects ||
        !qxl->render_update_cookie_num) {
        qemu_mutex_unlock(&qxl->ssd.lock);
        return;
    }
    trace_qxl_interface_update_area_complete(qxl->id, surface_id, dirty->left,
            dirty->right, dirty->top, dirty->bottom);
    trace_qxl_interface_update_area_complete_rest(qxl->id, num_updated_rects);
    if (qxl->num_dirty_rects + num_updated_rects > QXL_NUM_DIRTY_RECTS) {
        /*
         * overflow - treat this as a full update. Not expected to be common.
         */
        trace_qxl_interface_update_area_complete_overflow(qxl->id,
                                                          QXL_NUM_DIRTY_RECTS);
        qxl->guest_primary.resized = 1;
    }
    if (qxl->guest_primary.resized) {
        /*
         * Don't bother copying or scheduling the bh since we will flip
         * the whole area anyway on completion of the update_area async call
         */
        qemu_mutex_unlock(&qxl->ssd.lock);
        return;
    }
    qxl_i = qxl->num_dirty_rects;
    for (i = 0; i < num_updated_rects; i++) {
        qxl->dirty[qxl_i++] = dirty[i];
    }
    qxl->num_dirty_rects += num_updated_rects;
    trace_qxl_interface_update_area_complete_schedule_bh(qxl->id,
                                                         qxl->num_dirty_rects);
    qemu_bh_schedule(qxl->update_area_bh);
    qemu_mutex_unlock(&qxl->ssd.lock);
}