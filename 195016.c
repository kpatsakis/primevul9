static int rtl8139_pre_save(void *opaque)
{
    RTL8139State* s = opaque;
    int64_t current_time = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);

    /* for migration to older versions */
    s->TCTR = (current_time - s->TCTR_base) / PCI_PERIOD;
    s->rtl8139_mmio_io_addr_dummy = 0;

    return 0;
}