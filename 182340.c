static void vmxnet3_clear_interrupt(VMXNET3State *s, int int_idx)
{
    s->interrupt_states[int_idx].is_pending = false;
    if (s->auto_int_masking) {
        s->interrupt_states[int_idx].is_masked = true;
    }
    vmxnet3_update_interrupt_line_state(s, int_idx);
}