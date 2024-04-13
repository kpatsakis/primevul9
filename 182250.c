vmxnet3_on_interrupt_mask_changed(VMXNET3State *s, int lidx, bool is_masked)
{
    s->interrupt_states[lidx].is_masked = is_masked;
    vmxnet3_update_interrupt_line_state(s, lidx);
}