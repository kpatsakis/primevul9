vmxnet3_io_bar0_read(void *opaque, hwaddr addr, unsigned size)
{
    VMXNET3State *s = opaque;

    if (VMW_IS_MULTIREG_ADDR(addr, VMXNET3_REG_IMR,
                        VMXNET3_MAX_INTRS, VMXNET3_REG_ALIGN)) {
        int l = VMW_MULTIREG_IDX_BY_ADDR(addr, VMXNET3_REG_IMR,
                                         VMXNET3_REG_ALIGN);
        return s->interrupt_states[l].is_masked;
    }

    VMW_CBPRN("BAR0 unknown read [%" PRIx64 "], size %d", addr, size);
    return 0;
}