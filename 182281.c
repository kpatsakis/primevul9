vmxnet3_get_next_rx_descr(VMXNET3State *s, bool is_head,
                          struct Vmxnet3_RxDesc *descr_buf,
                          uint32_t *descr_idx,
                          uint32_t *ridx)
{
    if (is_head || !s->rx_packets_compound) {
        return vmxnet3_get_next_head_rx_descr(s, descr_buf, descr_idx, ridx);
    } else {
        return vmxnet3_get_next_body_rx_descr(s, descr_buf, descr_idx, ridx);
    }
}