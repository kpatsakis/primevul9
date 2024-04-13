static void vmxnet3_validate_interrupt_idx(bool is_msix, int idx)
{
    int max_ints = is_msix ? VMXNET3_MAX_INTRS : VMXNET3_MAX_NMSIX_INTRS;
    if (idx >= max_ints) {
        hw_error("Bad interrupt index: %d\n", idx);
    }
}