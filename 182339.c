vmxnet3_cleanup_msix(VMXNET3State *s)
{
    PCIDevice *d = PCI_DEVICE(s);

    if (s->msix_used) {
        vmxnet3_unuse_msix_vectors(s, VMXNET3_MAX_INTRS);
        msix_uninit(d, &s->msix_bar, &s->msix_bar);
    }
}