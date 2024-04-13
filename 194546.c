static inline void qxl_push_free_res(PCIQXLDevice *d, int flush)
{
    QXLReleaseRing *ring = &d->ram->release_ring;
    uint64_t *item;
    int notify;

#define QXL_FREE_BUNCH_SIZE 32

    if (ring->prod - ring->cons + 1 == ring->num_items) {
        /* ring full -- can't push */
        return;
    }
    if (!flush && d->oom_running) {
        /* collect everything from oom handler before pushing */
        return;
    }
    if (!flush && d->num_free_res < QXL_FREE_BUNCH_SIZE) {
        /* collect a bit more before pushing */
        return;
    }

    SPICE_RING_PUSH(ring, notify);
    trace_qxl_ring_res_push(d->id, qxl_mode_to_string(d->mode),
           d->guest_surfaces.count, d->num_free_res,
           d->last_release, notify ? "yes" : "no");
    trace_qxl_ring_res_push_rest(d->id, ring->prod - ring->cons,
           ring->num_items, ring->prod, ring->cons);
    if (notify) {
        qxl_send_events(d, QXL_INTERRUPT_DISPLAY);
    }
    SPICE_RING_PROD_ITEM(d, ring, item);
    if (!item) {
        return;
    }
    *item = 0;
    d->num_free_res = 0;
    d->last_release = NULL;
    qxl_ring_set_dirty(d);
}