static void pci_rtl8139_uninit(PCIDevice *dev)
{
    RTL8139State *s = RTL8139(dev);

    g_free(s->cplus_txbuffer);
    s->cplus_txbuffer = NULL;
    timer_free(s->timer);
    qemu_del_nic(s->nic);
}