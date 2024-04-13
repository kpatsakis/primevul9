vmxnet3_use_msix_vectors(VMXNET3State *s, int num_vectors)
{
    PCIDevice *d = PCI_DEVICE(s);
    int i;
    for (i = 0; i < num_vectors; i++) {
        int res = msix_vector_use(d, i);
        if (0 > res) {
            VMW_WRPRN("Failed to use MSI-X vector %d, error %d", i, res);
            vmxnet3_unuse_msix_vectors(s, i);
            return false;
        }
    }
    return true;
}