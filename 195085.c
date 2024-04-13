static void pci_rtl8139_realize(PCIDevice *dev, Error **errp)
{
    RTL8139State *s = RTL8139(dev);
    DeviceState *d = DEVICE(dev);
    uint8_t *pci_conf;

    pci_conf = dev->config;
    pci_conf[PCI_INTERRUPT_PIN] = 1;    /* interrupt pin A */
    /* TODO: start of capability list, but no capability
     * list bit in status register, and offset 0xdc seems unused. */
    pci_conf[PCI_CAPABILITY_LIST] = 0xdc;

    memory_region_init_io(&s->bar_io, OBJECT(s), &rtl8139_io_ops, s,
                          "rtl8139", 0x100);
    memory_region_init_alias(&s->bar_mem, OBJECT(s), "rtl8139-mem", &s->bar_io,
                             0, 0x100);

    pci_register_bar(dev, 0, PCI_BASE_ADDRESS_SPACE_IO, &s->bar_io);
    pci_register_bar(dev, 1, PCI_BASE_ADDRESS_SPACE_MEMORY, &s->bar_mem);

    qemu_macaddr_default_if_unset(&s->conf.macaddr);

    /* prepare eeprom */
    s->eeprom.contents[0] = 0x8129;
#if 1
    /* PCI vendor and device ID should be mirrored here */
    s->eeprom.contents[1] = PCI_VENDOR_ID_REALTEK;
    s->eeprom.contents[2] = PCI_DEVICE_ID_REALTEK_8139;
#endif
    s->eeprom.contents[7] = s->conf.macaddr.a[0] | s->conf.macaddr.a[1] << 8;
    s->eeprom.contents[8] = s->conf.macaddr.a[2] | s->conf.macaddr.a[3] << 8;
    s->eeprom.contents[9] = s->conf.macaddr.a[4] | s->conf.macaddr.a[5] << 8;

    s->nic = qemu_new_nic(&net_rtl8139_info, &s->conf,
                          object_get_typename(OBJECT(dev)), d->id, s);
    qemu_format_nic_info_str(qemu_get_queue(s->nic), s->conf.macaddr.a);

    s->cplus_txbuffer = NULL;
    s->cplus_txbuffer_len = 0;
    s->cplus_txbuffer_offset = 0;

    s->timer = timer_new_ns(QEMU_CLOCK_VIRTUAL, rtl8139_timer, s);
}