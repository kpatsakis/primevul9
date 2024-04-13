static inline void vmxnet3_ring_inc(Vmxnet3Ring *ring)
{
    if (++ring->next >= ring->size) {
        ring->next = 0;
        ring->gen ^= 1;
    }
}