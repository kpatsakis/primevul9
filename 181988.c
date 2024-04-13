static bool bitset_get(const unsigned long *bits, unsigned i) {
        return (bits[i / ULONG_BITS] >> (i % ULONG_BITS)) & 1UL;
}