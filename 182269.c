static inline void vmxnet3_ring_dec(Vmxnet3Ring *ring)
{
    if (ring->next-- == 0) {
        ring->next = ring->size - 1;
        ring->gen ^= 1;
    }
}