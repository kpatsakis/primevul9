static inline unsigned mph_hash(unsigned d, unsigned x) {
    x ^= d;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    return x;
}