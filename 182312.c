static void vmxnet3_reset_interrupt_states(VMXNET3State *s)
{
    int i;
    for (i = 0; i < ARRAY_SIZE(s->interrupt_states); i++) {
        s->interrupt_states[i].is_asserted = false;
        s->interrupt_states[i].is_pending = false;
        s->interrupt_states[i].is_masked = true;
    }
}