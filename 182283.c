static inline void vmxnet3_ring_init(PCIDevice *d,
                                     Vmxnet3Ring *ring,
                                     hwaddr pa,
                                     uint32_t size,
                                     uint32_t cell_size,
                                     bool zero_region)
{
    ring->pa = pa;
    ring->size = size;
    ring->cell_size = cell_size;
    ring->gen = VMXNET3_INIT_GEN;
    ring->next = 0;

    if (zero_region) {
        vmw_shmem_set(d, pa, 0, size * cell_size);
    }
}