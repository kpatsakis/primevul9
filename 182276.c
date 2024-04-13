vmxnet3_pci_dma_writev(PCIDevice *pci_dev,
                       const struct iovec *iov,
                       size_t start_iov_off,
                       hwaddr target_addr,
                       size_t bytes_to_copy)
{
    size_t curr_off = 0;
    size_t copied = 0;

    while (bytes_to_copy) {
        if (start_iov_off < (curr_off + iov->iov_len)) {
            size_t chunk_len =
                MIN((curr_off + iov->iov_len) - start_iov_off, bytes_to_copy);

            pci_dma_write(pci_dev, target_addr + copied,
                          iov->iov_base + start_iov_off - curr_off,
                          chunk_len);

            copied += chunk_len;
            start_iov_off += chunk_len;
            curr_off = start_iov_off;
            bytes_to_copy -= chunk_len;
        } else {
            curr_off += iov->iov_len;
        }
        iov++;
    }
}