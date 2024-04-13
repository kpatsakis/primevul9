static void bitset_put(unsigned long *bits, unsigned i) {
        bits[i / ULONG_BITS] |= (unsigned long) 1 << (i % ULONG_BITS);
}