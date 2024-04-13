static bool vmxnet3_interrupt_asserted(VMXNET3State *s, int lidx)
{
    return s->interrupt_states[lidx].is_asserted;
}