static void vmxnet3_validate_interrupts(VMXNET3State *s)
{
    int i;

    VMW_CFPRN("Verifying event interrupt index (%d)", s->event_int_idx);
    vmxnet3_validate_interrupt_idx(s->msix_used, s->event_int_idx);

    for (i = 0; i < s->txq_num; i++) {
        int idx = s->txq_descr[i].intr_idx;
        VMW_CFPRN("Verifying TX queue %d interrupt index (%d)", i, idx);
        vmxnet3_validate_interrupt_idx(s->msix_used, idx);
    }

    for (i = 0; i < s->rxq_num; i++) {
        int idx = s->rxq_descr[i].intr_idx;
        VMW_CFPRN("Verifying RX queue %d interrupt index (%d)", i, idx);
        vmxnet3_validate_interrupt_idx(s->msix_used, idx);
    }
}