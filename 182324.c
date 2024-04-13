static uint32_t vmxnet3_get_interrupt_config(VMXNET3State *s)
{
    uint32_t interrupt_mode = VMXNET3_IT_AUTO | (VMXNET3_IMM_AUTO << 2);
    VMW_CFPRN("Interrupt config is 0x%X", interrupt_mode);
    return interrupt_mode;
}