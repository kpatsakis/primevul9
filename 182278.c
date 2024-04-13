static inline hwaddr vmxnet3_ring_curr_cell_pa(Vmxnet3Ring *ring)
{
    return ring->pa + ring->next * ring->cell_size;
}