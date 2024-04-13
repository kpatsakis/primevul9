vmxnet3_get_next_body_rx_descr(VMXNET3State *s,
                               struct Vmxnet3_RxDesc *d,
                               uint32_t *didx,
                               uint32_t *ridx)
{
    vmxnet3_read_next_rx_descr(s, RXQ_IDX, RX_HEAD_BODY_RING, d, didx);

    /* Try to find corresponding descriptor in head/body ring */
    if (d->gen == vmxnet3_get_rx_ring_gen(s, RXQ_IDX, RX_HEAD_BODY_RING)) {
        /* Only read after generation field verification */
        smp_rmb();
        /* Re-read to be sure we got the latest version */
        vmxnet3_read_next_rx_descr(s, RXQ_IDX, RX_HEAD_BODY_RING, d, didx);
        if (d->btype == VMXNET3_RXD_BTYPE_BODY) {
            vmxnet3_inc_rx_consumption_counter(s, RXQ_IDX, RX_HEAD_BODY_RING);
            *ridx = RX_HEAD_BODY_RING;
            return true;
        }
    }

    /*
     * If there is no free descriptors on head/body ring or next free
     * descriptor is a head descriptor switch to body only ring
     */
    vmxnet3_read_next_rx_descr(s, RXQ_IDX, RX_BODY_ONLY_RING, d, didx);

    /* If no more free descriptors - return */
    if (d->gen == vmxnet3_get_rx_ring_gen(s, RXQ_IDX, RX_BODY_ONLY_RING)) {
        /* Only read after generation field verification */
        smp_rmb();
        /* Re-read to be sure we got the latest version */
        vmxnet3_read_next_rx_descr(s, RXQ_IDX, RX_BODY_ONLY_RING, d, didx);
        assert(d->btype == VMXNET3_RXD_BTYPE_BODY);
        *ridx = RX_BODY_ONLY_RING;
        vmxnet3_inc_rx_consumption_counter(s, RXQ_IDX, RX_BODY_ONLY_RING);
        return true;
    }

    return false;
}