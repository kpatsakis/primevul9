void qxl_spice_update_area(PCIQXLDevice *qxl, uint32_t surface_id,
                           struct QXLRect *area, struct QXLRect *dirty_rects,
                           uint32_t num_dirty_rects,
                           uint32_t clear_dirty_region,
                           qxl_async_io async, struct QXLCookie *cookie)
{
    trace_qxl_spice_update_area(qxl->id, surface_id, area->left, area->right,
                                area->top, area->bottom);
    trace_qxl_spice_update_area_rest(qxl->id, num_dirty_rects,
                                     clear_dirty_region);
    if (async == QXL_SYNC) {
        spice_qxl_update_area(&qxl->ssd.qxl, surface_id, area,
                        dirty_rects, num_dirty_rects, clear_dirty_region);
    } else {
        assert(cookie != NULL);
        spice_qxl_update_area_async(&qxl->ssd.qxl, surface_id, area,
                                    clear_dirty_region, (uintptr_t)cookie);
    }
}