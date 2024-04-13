static inline void encode_ones(Encoder *encoder, unsigned int n)
{
    unsigned int count;

    for (count = n >> 5; count; count--) {
        encode_32(encoder, ~0U);
    }

    if ((n &= 0x1f)) {
        encode(encoder, (1U << n) - 1, n);
    }
}