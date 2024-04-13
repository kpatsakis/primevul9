vmxnet3_unuse_msix_vectors(VMXNET3State *s, int num_vectors)
{
    PCIDevice *d = PCI_DEVICE(s);
    int i;
    for (i = 0; i < num_vectors; i++) {
        msix_vector_unuse(d, i);
    }
}