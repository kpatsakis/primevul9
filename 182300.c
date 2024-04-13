static inline size_t vmxnet3_ring_curr_cell_idx(Vmxnet3Ring *ring)
{
    return ring->next;
}